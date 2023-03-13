// Copyright 2023 by Tanner Swett and Medallion Instrumentation Systems.
//
// This file is part of Poutine. Poutine is free software; you can redistribute
// it and/or modify it under the terms of version 3 of the GNU General Public
// License as published by the Free Software Foundation.
//
// Poutine is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// rawheap.h: Unchecked functions for modifying the heap

#ifndef RAWHEAP_H
#define RAWHEAP_H

#include "heap.h"

// Allocate a new cell, or return -1 if none available
//
// If a cell is successfully allocated, then the newly allocated cell has a tag
// of ATOM, a car of -1, and a reference count of 1. If no cells are available,
// nothing happens and the function returns -1.
int alloc_cell(heap_p heap);

// Set the value of a field in a cell
void setfield(heap_p heap, int field, int index, int value);
// Add 1 to the reference count of a cell
void inc_refcount(heap_p heap, int index);
// Subtract 1 from the reference count of a cell
void dec_refcount(heap_p heap, int index);

// Make a cell into an atom and set its text
void setatom(heap_p heap, int index, const char *text);

#endif
