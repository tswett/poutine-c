// Copyright 2022 by Tanner Swett and Medallion Instrumentation Systems.
//
// This file is part of Poutine. Poutine is free software; you can redistribute
// it and/or modify it under the terms of version 3 of the GNU General Public
// License as published by the Free Software Foundation.
//
// Poutine is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// heap.h: The in-memory database ("heap") and functions for interacting with it

#ifndef HEAP_H
#define HEAP_H

// Get the value of a field in a cell
int getfield(int field, int index);
// Set the value of a field in a cell
void setfield(int field, int index, int value);
#define HEAP_SIZE (1024*1024)
#define FIELD_CAR 0
#define FIELD_CDR 1

#endif
