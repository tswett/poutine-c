// Copyright 2023 by Tanner Swett and Medallion Instrumentation Systems.
//
// This file is part of Poutine. Poutine is free software; you can redistribute
// it and/or modify it under the terms of version 3 of the GNU General Public
// License as published by the Free Software Foundation.
//
// Poutine is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// rcheap.h: Heap functions that respect reference counts

// This header file provides functions for manipulating cells in a way that
// respects reference counts and allocations. Specifically, the reference count
// of a cell must actually equal the number of references to it. A cell cannot
// be used unless it is allocated, and the collection of all unallocated cells
// is kept track of correctly.

// In the future, there will be garbage collection: cells will automatically be
// deallocated when there are no more references to them.

#ifndef RCHEAP_H
#define RCHEAP_H

typedef struct heap *heap_p;

// Get the value of a field in a cell
int rc_getfield(heap_p heap, int field, int index);
// Check if a cell contains a value
int rc_is_valid(heap_p heap, int index);
// Check if a cell has zero incoming references
int rc_is_unowned(heap_p heap, int index);

// Erase a cell, but leave it allocated
void rc_erase(heap_p heap, int index);
// Erase and free a cell
void rc_free(heap_p heap, int index);
// Make a cell into an atom and set its text
void rc_setatom(heap_p heap, int index, const char *text);
// Make a cell into a cons cell with the given car and cdr
void rc_setcons(heap_p heap, int index, int car, int cdr);
// Allocate a cell as an atom, returning -1 on insufficient space
int rc_atom(heap_p heap, const char *text);
// Allocate a cell as a cons cell, returning -1 on insufficient space
int rc_cons(heap_p heap, int car, int cdr);

#endif
