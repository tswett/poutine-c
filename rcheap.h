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

#ifndef RCHEAP_H
#define RCHEAP_H

typedef struct heap *heap_p;

// Check if a cell has zero incoming references
int rc_is_unowned(heap_p heap, int index);
// Make a cell into an atom and set its text
void rc_setatom(heap_p heap, int index, const char *text);

#endif
