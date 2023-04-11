// Copyright 2023 by Tanner Swett and Medallion Instrumentation Systems.
//
// This file is part of Poutine. Poutine is free software; you can redistribute
// it and/or modify it under the terms of version 3 of the GNU General Public
// License as published by the Free Software Foundation.
//
// Poutine is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// heap.h: The in-memory database ("heap") and functions for interacting with it
// rawheap.h: Unchecked functions for modifying the heap

#include <string.h>

#include "heap.h"
#include "panic.h"
#include "rawheap.h"

// Try to find an atom in the buffer; return 0 if it isn't there
int try_find_atom(heap_p heap, const char *text, char **result);

typedef struct cons_cell {
    int car;
    int cdr;
    int tag;
    int ref_count;
} cons_cell;

typedef struct heap {
    cons_cell *cells;
    size_t cell_count;
    int next_freed;
    int next_uninit;

    char *atom_text_buf;
    char *atom_text_next;
    char *atom_text_end;
    size_t atom_buf_size;
} heap;

heap_p malloc_heap(size_t cell_count, size_t atom_buf_size) {
    heap_p new_heap = calloc(1, sizeof(heap));
    if (!new_heap)
        PANIC("Failed to allocate enough memory for the heap");

    new_heap->cells = calloc(cell_count, sizeof(cons_cell));
    if (!new_heap->cells)
        PANIC("Failed to allocate enough memory for the heap");
    new_heap->cell_count = cell_count;

    new_heap->next_freed = -1;

    new_heap->next_uninit = 0;

    new_heap->atom_text_buf = calloc(atom_buf_size, sizeof(char));
    if (!new_heap->atom_text_buf)
        PANIC("Failed to allocate enough memory for the heap");
    new_heap->atom_text_next = new_heap->atom_text_buf;
    new_heap->atom_buf_size = atom_buf_size;

    return new_heap;
}

void free_heap(heap_p heap) {
    free(heap->atom_text_buf);
    free(heap->cells);
    free(heap);
}



int cell_count(heap_p heap) {
    return heap->cell_count;
}

int alloc_cell(heap_p heap) {
    int index;

    if (heap->next_freed != -1) {
        index = heap->next_freed;
        // pop this off the freed stack
        heap->next_freed = getfield(heap, FIELD_CAR, heap->next_freed);
    } else {
        index = heap->next_uninit;

        if (index >= heap->cell_count)
            return -1;

        heap->next_uninit++;
    }

    setfield(heap, FIELD_TAG, index, TAG_ATOM);
    setfield(heap, FIELD_CAR, index, -1);
    setfield(heap, FIELD_REFCOUNT, index, 0);

    return index;
}

void free_cell(heap_p heap, int index) {
    int tag = getfield(heap, FIELD_TAG, index);
    if (tag == TAG_UNINIT)
        PANIC("tried to free an uninitialized cell");
    if (tag == TAG_FREED)
        PANIC("tried to free a freed cell");

    // push this onto the freed stack
    setfield(heap, FIELD_TAG, index, TAG_FREED);
    setfield(heap, FIELD_CAR, index, heap->next_freed);

    heap->next_freed = index;
}

int getfield(heap_p heap, int field, int index) {
    if (index < 0 || index >= heap->cell_count) {
        PANIC("Index out of range: %d", index);
    }

    switch (field) {
        case FIELD_CAR:
            return heap->cells[index].car;
        case FIELD_CDR:
            return heap->cells[index].cdr;
        case FIELD_TAG:
            return heap->cells[index].tag;
        case FIELD_REFCOUNT:
            return heap->cells[index].ref_count;
        default:
            PANIC("Unrecognized field number: %d", field);
    }
}

void setfield(heap_p heap, int field, int index, int value) {
    if (index < 0 || index >= heap->cell_count) {
        PANIC("Index out of range: %d", index);
    }

    switch (field) {
        case FIELD_CAR:
            heap->cells[index].car = value;
            return;
        case FIELD_CDR:
            heap->cells[index].cdr = value;
            return;
        case FIELD_TAG:
            heap->cells[index].tag = value;
            return;
        case FIELD_REFCOUNT:
            heap->cells[index].ref_count = value;
            return;
        default:
            PANIC("Unrecognized field number: %d", field);
    }
}

void inc_refcount(heap_p heap, int index) {
    int refcount = getfield(heap, FIELD_REFCOUNT, index);
    setfield(heap, FIELD_REFCOUNT, index, refcount + 1);
}

void dec_refcount(heap_p heap, int index) {
    int refcount = getfield(heap, FIELD_REFCOUNT, index);
    setfield(heap, FIELD_REFCOUNT, index, refcount - 1);
}

int isatom(heap_p heap, int index) {
    if (index < 0 || index >= heap->cell_count)
        PANIC("Index out of range: %d", index);

    if (heap->cells[index].tag != TAG_ATOM)
        return 0;

    if (index < 0 || index >= heap->atom_buf_size)
        return 0;

    if (heap->atom_text_buf[heap->cells[index].car] == 0)
        return 0;

    return 1;
}

const char *getatom(heap_p heap, int index) {
    if (index < 0 || index >= heap->cell_count)
        PANIC("Index out of range: %d", index);

    if (heap->cells[index].tag != TAG_ATOM)
        PANIC("Cell %d is not an atom", index);

    if (index < 0 || index >= heap->atom_buf_size)
        PANIC("Atom text index out of range: %d", index);

    int buf_index = heap->cells[index].car;

    if (heap->atom_text_buf[buf_index] == 0)
        PANIC("Atom text index points at a null byte: %d", index);

    return &(heap->atom_text_buf[buf_index]);
}

void setatom(heap_p heap, int index, const char *text) {
    if (index < 0 || index >= heap->cell_count)
        PANIC("Index out of range: %d", index);

    if (*text == 0)
        PANIC("The given atom text was empty");

    heap->cells[index].tag = TAG_ATOM;

    char *text_location;
    int found_it = try_find_atom(heap, text, &text_location);

    if (!found_it) {
        int space_needed = strlen(text) + 1;
        char *atom_text_end = heap->atom_text_buf + heap->atom_buf_size;

        if (atom_text_end - heap->atom_text_next < space_needed)
            PANIC("Ran out of space in the atom text buffer");

        text_location = heap->atom_text_next;
        strcpy(text_location, text);

        heap->atom_text_next += space_needed;
    }

    heap->cells[index].car = text_location - heap->atom_text_buf;
}

int try_find_atom(heap_p heap, const char *text, char **result) {
    char *cursor = heap->atom_text_buf;

    while (*cursor != 0) {
        if (strcmp(cursor, text) == 0) {
            *result = cursor;
            return 1;
        } else {
            cursor += strlen(cursor) + 1;
        }
    }

    return 0;
}



int print_to_buffer(heap_p heap, int index, char *buffer, int length) {
    const char *atom = getatom(heap, index);
    if (!atom)
        PANIC("The value at index %d isn't an atom", index);

    if (strcmp(atom, "nil") == 0)
        atom = "()";

    if (length >= strlen(atom) + 1) {
        strncpy(buffer, atom, length);
        return 1;
    } else {
        return 0;
    }
}
