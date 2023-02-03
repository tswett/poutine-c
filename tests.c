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



// Try out the heap functions.
void test_heap(void);
// Try out the functions that deal with atoms.
void test_atoms(void);



#define RUN_TEST(test, ...) do { \
    printf("Running %s...\n", #test); \
    test(__VA_ARGS__); \
} while (0)

int main(int argc, char **argv) {
    RUN_TEST(test_heap);
    RUN_TEST(test_atoms);
    printf("Everything looks good.\n");
}

#define TEST_HEAP_EXPECT_FIELD(field, index, expected) do { \
    int actual = getfield(heap, field, index); \
    if (actual != expected) { \
        PANIC("Unexpected result from getfield(%s, %d): expected %d, got %d", \
            #field, index, expected, actual); \
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

    TEST_HEAP_EXPECT_FIELD(FIELD_CAR, 0, 53);
    TEST_HEAP_EXPECT_FIELD(FIELD_CDR, 0, 58);
    TEST_HEAP_EXPECT_FIELD(FIELD_TAG, 0, TAG_CONS);
    TEST_HEAP_EXPECT_FIELD(FIELD_CAR, 1, 59);
    TEST_HEAP_EXPECT_FIELD(FIELD_CDR, 1, 26);
    TEST_HEAP_EXPECT_FIELD(FIELD_TAG, 1, TAG_ATOM);
    TEST_HEAP_EXPECT_FIELD(FIELD_TAG, 2, TAG_UNINIT);

    free_heap(heap);
}

void test_atoms() {
    heap_p heap = malloc_heap(2, 7);

    setatom(heap, 0, "nil");
    setatom(heap, 1, "ha");

    TEST_HEAP_EXPECT_FIELD(FIELD_TAG, 0, TAG_ATOM);
    const char *atom = getatom(heap, 0);

    if (strcmp(atom, "nil") != 0)
        PANIC("Unexpected result from getatom(0): expected \"nil\", got \"%s\"", atom);

    TEST_HEAP_EXPECT_FIELD(FIELD_TAG, 1, TAG_ATOM);
    atom = getatom(heap, 1);

    if (strcmp(atom, "ha") != 0)
        PANIC("Unexpected result from getatom(1): expected \"ha\", got \"%s\"", atom);

    free_heap(heap);
}
