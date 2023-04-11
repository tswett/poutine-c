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
// Try out the allocating heap functions.
void test_rcheap_alloc(void);
// Try out the print function.
void test_print(void);



#define RUN_TEST(test, ...) do { \
    printf("Running %s...\n", #test); \
    test(__VA_ARGS__); \
} while (0)

int main(int argc, char **argv) {
    RUN_TEST(test_heap);
    RUN_TEST(test_atoms);
    RUN_TEST(test_allocate);
    RUN_TEST(test_rcheap);
    RUN_TEST(test_rcheap_alloc);
    RUN_TEST(test_print);
    printf("Everything looks good.\n");
}

#define EXPECT(type, expr, expected) do { \
    type EXPECT_actual = (expr); \
    if (EXPECT_actual != (expected)) { \
        PANIC("Unexpected result from %s %s: expected %d, got %d", \
            #type, #expr, (expected), EXPECT_actual); \
    } \
} while (0)

#define EXPECT_STR(expr, expected) do { \
    const char *EXPECT_STR_actual = (expr); \
    if (strcmp(EXPECT_STR_actual, (expected)) != 0) { \
        PANIC("Unexpected result from string %s: expected \"%s\", got \"%s\"", \
            #expr, (expected), EXPECT_STR_actual); \
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
    EXPECT_STR(getatom(heap, 0), "nil");

    EXPECT(int, getfield(heap, FIELD_TAG, 1), TAG_ATOM);
    EXPECT_STR(getatom(heap, 1), "ha");

    free_heap(heap);
}

void test_allocate() {
    heap_p heap = malloc_heap(3, 1);

    int indices[3];

    for (int i = 0; i < 3; i++) {
        int index = alloc_cell(heap);
        indices[i] = index;

        EXPECT(int, getfield(heap, FIELD_TAG, index), TAG_ATOM);
        EXPECT(int, getfield(heap, FIELD_REFCOUNT, index), 0);
        EXPECT(int, getfield(heap, FIELD_CAR, index), -1);

        setfield(heap, FIELD_CAR, index, i + 100);
    }

    // Now it should be all out of cells, so the next allocation should fail.
    EXPECT(int, alloc_cell(heap), -1);

    for (int i = 0; i < 3; i++) {
        EXPECT(int, getfield(heap, FIELD_CAR, indices[i]), i + 100);
    }

    int test_indices[3] = {1, 2, 0};

    for (int i = 0; i < 3; i++) {
        // If memory is full, then we free a cell and allocate a cell, we should
        // get back the cell we just freed.
        free_cell(heap, test_indices[i]);

        EXPECT(int, getfield(heap, FIELD_TAG, test_indices[i]), TAG_FREED);

        int index = alloc_cell(heap);

        EXPECT(int, index, test_indices[i]);
    }

    for (int i = 0; i < 3; i++) {
        free_cell(heap, i);
    }

    for (int i = 0; i < 3; i++) {
        int index = alloc_cell(heap);
        EXPECT(int, getfield(heap, FIELD_TAG, index), TAG_ATOM);
    }

    // Now it should be all out of cells again, so the next allocation should
    // fail once more.
    EXPECT(int, alloc_cell(heap), -1);

    free_heap(heap);
}

void test_rcheap() {
    heap_p heap = malloc_heap(10, 20);

    EXPECT(int, rc_is_unowned(heap, 0), 1);
    EXPECT(int, rc_is_valid(heap, 0), 0);
    EXPECT(int, rc_is_valid(heap, -1), 0);
    EXPECT(int, rc_is_valid(heap, 10), 0);

    rc_setatom(heap, 1, "red");
    rc_setatom(heap, 2, "nil");

    EXPECT(int, rc_is_valid(heap, 1), 1);
    EXPECT(int, rc_getfield(heap, FIELD_TAG, 1), TAG_ATOM);
    EXPECT(int, rc_is_unowned(heap, 1), 1);
    EXPECT(int, rc_is_unowned(heap, 2), 1);

    rc_setcons(heap, 3, 1, 2);

    EXPECT(int, rc_is_valid(heap, 3), 1);
    EXPECT(int, rc_getfield(heap, FIELD_TAG, 3), TAG_CONS);
    EXPECT(int, rc_getfield(heap, FIELD_CAR, 3), 1);
    EXPECT(int, rc_getfield(heap, FIELD_CDR, 3), 2);
    EXPECT(int, rc_is_unowned(heap, 1), 0);
    EXPECT(int, rc_is_unowned(heap, 2), 0);

    rc_setatom(heap, 3, "orange");

    EXPECT(int, rc_is_unowned(heap, 1), 1);
    EXPECT(int, rc_is_unowned(heap, 2), 1);

    free_heap(heap);
}

void test_rcheap_alloc() {
    heap_p heap = malloc_heap(3, 30);

    int nil = rc_atom(heap, "nil");
    int red = rc_atom(heap, "red");
    int orange = rc_atom(heap, "orange");

    // The heap is full, so this next allocation should fail.
    int yellow = rc_atom(heap, "yellow");

    EXPECT(int, rc_getfield(heap, FIELD_TAG, nil), TAG_ATOM);
    EXPECT_STR(getatom(heap, nil), "nil");
    EXPECT(int, rc_getfield(heap, FIELD_TAG, red), TAG_ATOM);
    EXPECT_STR(getatom(heap, red), "red");
    EXPECT(int, rc_getfield(heap, FIELD_TAG, orange), TAG_ATOM);
    EXPECT_STR(getatom(heap, orange), "orange");

    EXPECT(int, yellow, -1);

    rc_free(heap, orange);

    int list1 = rc_cons(heap, red, nil);

    // The heap is full again, so this next allocation should fail.
    int list2 = rc_cons(heap, red, list1);

    EXPECT(int, rc_getfield(heap, FIELD_TAG, list1), TAG_CONS);
    EXPECT(int, rc_getfield(heap, FIELD_CAR, list1), red);
    EXPECT(int, rc_getfield(heap, FIELD_CDR, list1), nil);

    EXPECT(int, list2, -1);

    free_heap(heap);
}

#define EXPECT_STR(expr, expected) do { \
    const char *EXPECT_STR_actual = (expr); \
    if (strcmp(EXPECT_STR_actual, (expected)) != 0) { \
        PANIC("Unexpected result from string %s: expected \"%s\", got \"%s\"", \
            #expr, (expected), EXPECT_STR_actual); \
    } \
} while (0)

#define PRINT_BUFFER_SIZE 50

#define EXPECT_PRINT(index, result) do { \
    EXPECT(int, print_to_buffer(heap, (index), buffer, PRINT_BUFFER_SIZE), 1); \
    EXPECT_STR(buffer, (result)); \
    EXPECT(int, print_to_buffer(heap, (index), buffer, sizeof(result) - 1), 0); \
    EXPECT(int, print_to_buffer(heap, (index), buffer, sizeof(result)), 1); \
    EXPECT_STR(buffer, (result)); \
} while (0)

void test_print() {
    heap_p heap = malloc_heap(10, 30);
    char buffer[PRINT_BUFFER_SIZE];

    int nil = rc_atom(heap, "nil");
    int red = rc_atom(heap, "red");
    int orange = rc_atom(heap, "orange");

    EXPECT_PRINT(red, "red");
    EXPECT_PRINT(orange, "orange");
    EXPECT_PRINT(nil, "()");
}
