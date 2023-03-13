// Copyright 2023 by Tanner Swett and Medallion Instrumentation Systems.
//
// This file is part of Poutine. Poutine is free software; you can redistribute
// it and/or modify it under the terms of version 3 of the GNU General Public
// License as published by the Free Software Foundation.
//
// Poutine is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// tests.c: Some automated tests

#include <stdio.h>
#include <string.h>

#include "heap.h"
#include "panic.h"
#include "rawheap.h"
#include "rcheap.h"



// Try out the heap functions.
void test_heap(void);
// Try out the functions that deal with atoms.
void test_atoms(void);
// Try out allocating cells.
void test_allocate(void);
// Try out the reference-counting heap functions.
void test_rcheap(void);



#define RUN_TEST(test, ...) do { \
    printf("Running %s...\n", #test); \
    test(__VA_ARGS__); \
} while (0)

int main(int argc, char **argv) {
    RUN_TEST(test_heap);
    RUN_TEST(test_atoms);
    RUN_TEST(test_allocate);
    RUN_TEST(test_rcheap);
    printf("Everything looks good.\n");
}

#define EXPECT(type, expr, expected) do { \
    type EXPECT_actual = (expr); \
    if (EXPECT_actual != (expected)) { \
        PANIC("Unexpected result from %s: expected %d, got %d", \
            #expr, expected, EXPECT_actual); \
    } \
} while (0)

void test_heap() {
    heap_p heap = malloc_heap(3, 1);

    setfield(heap, FIELD_CAR, 0, 31);
    setfield(heap, FIELD_CDR, 0, 41);
    setfield(heap, FIELD_TAG, 0, TAG_ATOM);
    setfield(heap, FIELD_CAR, 1, 59);
    setfield(heap, FIELD_CDR, 1, 26);
    setfield(heap, FIELD_TAG, 1, TAG_ATOM);
    setfield(heap, FIELD_CAR, 0, 53);
    setfield(heap, FIELD_CDR, 0, 58);
    setfield(heap, FIELD_TAG, 0, TAG_CONS);

    EXPECT(int, getfield(heap, FIELD_CAR, 0), 53);
    EXPECT(int, getfield(heap, FIELD_CDR, 0), 58);
    EXPECT(int, getfield(heap, FIELD_TAG, 0), TAG_CONS);
    EXPECT(int, getfield(heap, FIELD_CAR, 1), 59);
    EXPECT(int, getfield(heap, FIELD_CDR, 1), 26);
    EXPECT(int, getfield(heap, FIELD_TAG, 1), TAG_ATOM);
    EXPECT(int, getfield(heap, FIELD_TAG, 2), TAG_UNINIT);

    free_heap(heap);
}

void test_atoms() {
    heap_p heap = malloc_heap(2, 7);

    setatom(heap, 0, "nil");
    setatom(heap, 1, "ha");

    EXPECT(int, getfield(heap, FIELD_TAG, 0), TAG_ATOM);
    const char *atom = getatom(heap, 0);

    if (strcmp(atom, "nil") != 0)
        PANIC("Unexpected result from getatom(0): expected \"nil\", got \"%s\"", atom);

    EXPECT(int, getfield(heap, FIELD_TAG, 1), TAG_ATOM);
    atom = getatom(heap, 1);

    if (strcmp(atom, "ha") != 0)
        PANIC("Unexpected result from getatom(1): expected \"ha\", got \"%s\"", atom);

    free_heap(heap);
}

void test_allocate() {
    heap_p heap = malloc_heap(3, 1);

    int indices[3];

    for (int i = 0; i < 3; i++) {
        int index = alloc_cell(heap);
        indices[i] = index;

        EXPECT(int, getfield(heap, FIELD_TAG, index), TAG_ATOM);
        EXPECT(int, getfield(heap, FIELD_REFCOUNT, index), 1);
        EXPECT(int, getfield(heap, FIELD_CAR, index), -1);

        setfield(heap, FIELD_CAR, index, i + 100);
    }

    // Now it should be all out of cells, so the next allocation should fail.
    EXPECT(int, alloc_cell(heap), -1);

    for (int i = 0; i < 3; i++) {
        EXPECT(int, getfield(heap, FIELD_CAR, indices[i]), i + 100);
    }
}

void test_rcheap() {
    heap_p heap = malloc_heap(10, 20);

    EXPECT(int, rc_is_unowned(heap, 0), 1);
    EXPECT(int, rc_is_valid(heap, 0), 0);

    rc_setatom(heap, 1, "red");
    rc_setatom(heap, 2, "nil");

    EXPECT(int, rc_getfield(heap, FIELD_TAG, 1), TAG_ATOM);
    EXPECT(int, rc_is_unowned(heap, 1), 1);
    EXPECT(int, rc_is_unowned(heap, 2), 1);

    rc_setcons(heap, 3, 1, 2);

    EXPECT(int, rc_getfield(heap, FIELD_TAG, 3), TAG_CONS);
    EXPECT(int, rc_getfield(heap, FIELD_CAR, 3), 1);
    EXPECT(int, rc_getfield(heap, FIELD_CDR, 3), 2);
    EXPECT(int, rc_is_unowned(heap, 1), 0);
    EXPECT(int, rc_is_unowned(heap, 2), 0);

    rc_setatom(heap, 3, "orange");

    EXPECT(int, rc_is_unowned(heap, 1), 1);
    EXPECT(int, rc_is_unowned(heap, 2), 1);
}
