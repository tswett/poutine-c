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
CFLAGS = -g -Wall -Werror-implicit-function-declaration -fdiagnostics-color=always

all: bin/poutine bin/test

run: bin/poutine
	bin/poutine

test: bin/test
	bin/test

bin/poutine: bin/heap.o bin/main.o
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/poutine bin/heap.o bin/main.o

bin/test: bin/heap.o bin/rcheap.o bin/tests.o
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/test bin/heap.o bin/rcheap.o bin/tests.o

bin/%.o: %.c *.h
	mkdir -p bin
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f bin/poutine bin/test bin/heap.o bin/main.o bin/rawheap.o bin/rcheap.o bin/tests.o
