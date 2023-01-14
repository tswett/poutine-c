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

#include "heap.h"
#include "panic.h"



// Try out the heap functions.
void test_heap(void);



int main(int argc, char **argv) {
    test_heap();
    printf("Everything looks good.\n");
}

#define TEST_HEAP_EXPECT_FIELD(field, index, expected) do { \
    int actual = getfield(field, index); \
    if (actual != expected) { \
        PANIC("Unexpected result from getfield(%s, %d): expected %d, got %d", \
            #field, index, expected, actual); \
    } \
} while (0)

void test_heap() {
    setfield(FIELD_CAR, 0, 31);
    setfield(FIELD_CDR, 0, 41);
    setfield(FIELD_TAG, 0, TAG_ATOM);
    setfield(FIELD_CAR, 1, 59);
    setfield(FIELD_CDR, 1, 26);
    setfield(FIELD_TAG, 1, TAG_ATOM);
    setfield(FIELD_CAR, 0, 53);
    setfield(FIELD_CDR, 0, 58);
    setfield(FIELD_TAG, 0, TAG_CONS);

    TEST_HEAP_EXPECT_FIELD(FIELD_CAR, 0, 53);
    TEST_HEAP_EXPECT_FIELD(FIELD_CDR, 0, 58);
    TEST_HEAP_EXPECT_FIELD(FIELD_TAG, 0, TAG_CONS);
    TEST_HEAP_EXPECT_FIELD(FIELD_CAR, 1, 59);
    TEST_HEAP_EXPECT_FIELD(FIELD_CDR, 1, 26);
    TEST_HEAP_EXPECT_FIELD(FIELD_TAG, 1, TAG_ATOM);
    TEST_HEAP_EXPECT_FIELD(FIELD_TAG, 2, TAG_UNINIT);
}
