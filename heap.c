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

#include "heap.h"
#include "panic.h"

typedef struct cons_cell {
    int car;
    int cdr;
    int tag;
    int refCount;
} cons_cell;

cons_cell heap[HEAP_SIZE];

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
