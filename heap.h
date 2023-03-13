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

#ifndef HEAP_H
#define HEAP_H

typedef struct heap *heap_p;

// Allocate a heap with the given number of cons cells and atom buffer
// characters
//
// Use free_heap() to free the heap. This function panics if it fails to
// allocate enough memory.
heap_p malloc_heap(size_t cell_count, size_t atom_buf_size);

// Free a heap allocated with malloc_heap().
void free_heap(heap_p heap);

// Get the number of cells in the heap
int cell_count(heap_p heap);

// Get the value of a field in a cell
int getfield(heap_p heap, int field, int index);

// Return 1 if this cell is a valid atom, 0 otherwise
//
// A cell is a valid atom if its tag is TAG_ATOM and its car is a valid index
// into the atom text buffer.
int isatom(heap_p heap, int index);
// Get the text of an atom cell; return 0 if it isn't an atom
//
// The result pointer remains valid until the heap is freed.
const char *getatom(heap_p heap, int index);

#define FIELD_CAR 0
#define FIELD_CDR 1
#define FIELD_TAG 2
#define FIELD_REFCOUNT 3

#define TAG_UNINIT 0
#define TAG_ATOM 1
#define TAG_CONS 2

#endif
