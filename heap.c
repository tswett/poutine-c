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

#include <string.h>

#include "heap.h"
#include "panic.h"

// Try to find an atom in the buffer; return 0 if it isn't there
int try_find_atom(const char *text, char **result);

typedef struct cons_cell {
    int car;
    int cdr;
    int tag;
    int refCount;
} cons_cell;

cons_cell heap[HEAP_SIZE];

char atom_text_buf[ATOM_TEXT_SIZE];
char *atom_text_next = atom_text_buf;
char *const atom_text_end = atom_text_buf + ATOM_TEXT_SIZE;

int getfield(int field, int index) {
    if (index < 0 || index >= HEAP_SIZE) {
        PANIC("Index out of range: %d", index);
    }

    switch (field) {
        case FIELD_CAR:
            return heap[index].car;
        case FIELD_CDR:
            return heap[index].cdr;
        case FIELD_TAG:
            return heap[index].tag;
        default:
            PANIC("Unrecognized field number: %d", field);
    }
}

void setfield(int field, int index, int value) {
    if (index < 0 || index >= HEAP_SIZE) {
        PANIC("Index out of range: %d", index);
    }

    switch (field) {
        case FIELD_CAR:
            heap[index].car = value;
            return;
        case FIELD_CDR:
            heap[index].cdr = value;
            return;
        case FIELD_TAG:
            heap[index].tag = value;
            return;
        default:
            PANIC("Unrecognized field number: %d", field);
    }
}

int isatom(int index) {
    if (index < 0 || index >= HEAP_SIZE)
        PANIC("Index out of range: %d", index);

    if (heap[index].tag != TAG_ATOM)
        return 0;

    if (index < 0 || index >= ATOM_TEXT_SIZE)
        return 0;

    if (atom_text_buf[heap[index].car] == 0)
        return 0;

    return 1;
}

const char *getatom(int index) {
    if (index < 0 || index >= HEAP_SIZE)
        PANIC("Index out of range: %d", index);

    if (heap[index].tag != TAG_ATOM)
        PANIC("Cell %d is not an atom", index);

    if (index < 0 || index >= ATOM_TEXT_SIZE)
        PANIC("Atom text index out of range: %d", index);

    int buf_index = heap[index].car;

    if (atom_text_buf[buf_index] == 0)
        PANIC("Atom text index points at a null byte: %d", index);

    return &atom_text_buf[buf_index];
}

void setatom(int index, const char *text) {
    if (index < 0 || index >= HEAP_SIZE)
        PANIC("Index out of range: %d", index);

    if (*text == 0)
        PANIC("The given atom text was empty");

    heap[index].tag = TAG_ATOM;

    char *text_location;
    int found_it = try_find_atom(text, &text_location);

    if (!found_it) {
        int space_needed = strlen(text) + 1;
        if (atom_text_end - atom_text_next < space_needed)
            PANIC("Ran out of space in the atom text buffer");

        text_location = atom_text_next;
        strcpy(text_location, text);

        atom_text_next += space_needed;
    }

    heap[index].car = text_location - atom_text_buf;
}

int try_find_atom(const char *text, char **result) {
    char *cursor = atom_text_buf;

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
