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

int rc_getfield(heap_p heap, int field, int index) {
    return getfield(heap, field, index);
}

int rc_is_valid(heap_p heap, int index) {
    int tag = getfield(heap, FIELD_TAG, index);
    return tag == TAG_ATOM;
}

int rc_is_unowned(heap_p heap, int index) {
    int refcount = getfield(heap, FIELD_REFCOUNT, index);

    if (refcount < 0)
        PANIC("The cell at index %d has reference count %d, which is negative", index, refcount);

    return refcount == 0;
}

void rc_erase(heap_p heap, int index) {
    if (!rc_is_unowned(heap, index))
        PANIC("Tried to erase the cell at index %d which has references to it", index);

    if (getfield(heap, FIELD_TAG, index) == TAG_CONS) {
        int car = getfield(heap, FIELD_CAR, index);
        dec_refcount(heap, car);
        int cdr = getfield(heap, FIELD_CDR, index);
        dec_refcount(heap, cdr);
    }

    setfield(heap, FIELD_TAG, index, TAG_UNINIT);
}

void rc_setatom(heap_p heap, int index, const char *text) {
    rc_erase(heap, index);

    setatom(heap, index, text);
}

void rc_setcons(heap_p heap, int index, int car, int cdr) {
    rc_erase(heap, index);

    if (index == car)
        PANIC("Tried to create a cons cell at index %d whose car is a self-reference", index);

    if (index == cdr)
        PANIC("Tried to create a cons cell at index %d whose cdr is a self-reference", index);

    if (!rc_is_valid(heap, car))
        PANIC("Tried to create a reference to cell %d, which doesn't contain a value", car);

    if (!rc_is_valid(heap, cdr))
        PANIC("Tried to create a reference to cell %d, which doesn't contain a value", cdr);

    setfield(heap, FIELD_TAG, index, TAG_CONS);
    setfield(heap, FIELD_CAR, index, car);
    setfield(heap, FIELD_CDR, index, cdr);

    inc_refcount(heap, car);
    inc_refcount(heap, cdr);
}
