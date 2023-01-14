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

#ifndef HEAP_H
#define HEAP_H

// Get the value of a field in a cell
int getfield(int field, int index);
// Set the value of a field in a cell
void setfield(int field, int index, int value);

// Return 1 if this cell is a valid atom, 0 otherwise
//
// A cell is a valid atom if its tag is TAG_ATOM and its car is a valid index
// into the atom text buffer.
int isatom(int index);
// Get the text of an atom cell; return 0 if it isn't an atom
//
// The result pointer remains valid for the entire time this program is running.
const char *getatom(int index);
// Make a cell into an atom and set its text
void setatom(int index, const char *text);

#define HEAP_SIZE (1024*1024)
#define ATOM_TEXT_SIZE (1024*8)

#define FIELD_CAR 0
#define FIELD_CDR 1
#define FIELD_TAG 2

#define TAG_UNINIT 0
#define TAG_ATOM 1
#define TAG_CONS 2

#endif
