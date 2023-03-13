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

#include <string.h>

#include "heap.h"
#include "panic.h"
#include "rcheap.h"

int rc_is_unowned(heap_p heap, int index) {
    return getfield(heap, FIELD_REFCOUNT, index) == 0;
}

void rc_setatom(heap_p heap, int index, const char *text) {
    if (!rc_is_unowned(heap, index))
        PANIC("Tried to overwrite the cell at index %d which has references to it", index);

    setatom(heap, index, text);
}
