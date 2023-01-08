// Copyright 2023 by Tanner Swett and Medallion Instrumentation Systems.
//
// This file is part of Poutine. Poutine is free software; you can redistribute
// it and/or modify it under the terms of version 3 of the GNU General Public
// License as published by the Free Software Foundation.
//
// Poutine is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// panic.h: The PANIC macro

#ifndef PANIC_H
#define PANIC_H

#include <stdio.h>
#include <stdlib.h>

#define PANIC(message, ...) do { \
    fprintf(stderr, "PANIC at %s line %d: " message "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    exit(-1); \
} while (0)

#endif
