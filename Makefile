# Copyright 2023 by Tanner Swett and Medallion Instrumentation Systems.
#
# This file is part of Poutine. Poutine is free software; you can redistribute
# it and/or modify it under the terms of version 3 of the GNU General Public
# License as published by the Free Software Foundation.
#
# Poutine is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.

CC = gcc
CFLAGS = -g -Wall -fdiagnostics-color=always

all: bin/poutine bin/test

bin/poutine: bin/heap.o bin/main.o
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/poutine bin/heap.o bin/main.o

bin/test: bin/heap.o bin/tests.o
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/test bin/heap.o bin/tests.o

bin/heap.o: heap.c heap.h panic.h
	mkdir -p bin
	$(CC) $(CFLAGS) -c -o bin/heap.o heap.c

bin/main.o: main.c heap.h
	mkdir -p bin
	$(CC) $(CFLAGS) -c -o bin/main.o main.c

bin/tests.o: tests.c heap.h panic.h
	mkdir -p bin
	$(CC) $(CFLAGS) -c -o bin/tests.o tests.c

clean:
	rm -f bin/poutine bin/test bin/heap.o bin/main.o bin/tests.o