// Copyright 2023 by Tanner Swett and Medallion Instrumentation Systems.
//
// This file is part of Poutine. Poutine is free software; you can redistribute
// it and/or modify it under the terms of version 3 of the GNU General Public
// License as published by the Free Software Foundation.
//
// Poutine is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"
#include "panic.h"



// Command parsing and processing:

// Read a command from the user and run it
void process_command(void);

// Individual commands:

// Get and print the value of a field in a cell
void cmd_getfield(int field, const char *command_name);
// Set the value of a field in a cell
void cmd_setfield(int field, const char *command_name);

// Get and print the tag of a cell
void cmd_gettag(void);
// Set the tag of a cell
void cmd_settag(void);

// Get and print the text of an atom
void cmd_getatom(void);
// Set the text of an atom
void cmd_setatom(void);

// Error messages:

// Print "Unrecognized command: %s"
void unknown_command(const char *command_name);
// Print "Too few arguments to %s"
void too_few_arguments(const char *command_name);
// Print "Too many arguments to %s"
void too_many_arguments(const char *command_name);
// Print "Invalid number: %s"
void invalid_number(const char *word);
// Print "Index out of range: %d"
void index_out_of_range(int index);
// Print "Unrecognized tag name: %s"
void unknown_tagname(const char *word);
// Print "The cell at index %d is not a valid atom"
void not_an_atom(int index);

// Argument parsing using strtok:

// Try to get an int argument; return 0 on failure
int get_int_argument_strtok(const char *command_name, int *result);
// Try to get a word argument; return 0 on failure
//
// The result string remains valid for as long as the command string is valid.
int get_word_argument_strtok(const char *command_name, const char **result);
// Try to get a tag name argument; return 0 on failure
int get_tagname_argument_strtok(const char *command_name, int *result);
// Assert that there are no more arguments; return 0 if there is one
int no_more_arguments_strtok(const char *command_name);



#define HEAP_SIZE (1024*1024)
#define ATOM_TEXT_SIZE (1024*8)

heap_p heap;



// Command parsing and processing:

int main(int argc, char **argv) {
    heap = malloc_heap(HEAP_SIZE, ATOM_TEXT_SIZE);

    while (!feof(stdin)) {
        process_command();
    }

    fprintf(stderr, "\n");
}

void process_command(void) {
    char command[1024];

    fprintf(stderr, "> ");
    if (!fgets(command, sizeof(command), stdin))
        return;

    const char *command_name = strtok(command, " \n");

    if (!command_name)
        return;

    if (strcmp(command_name, "getcar") == 0)
        cmd_getfield(FIELD_CAR, command_name);
    else if (strcmp(command_name, "getcdr") == 0)
        cmd_getfield(FIELD_CDR, command_name);
    else if (strcmp(command_name, "gettag") == 0)
        cmd_gettag();
    else if (strcmp(command_name, "getatom") == 0)
        cmd_getatom();
    else if (strcmp(command_name, "setcar") == 0)
        cmd_setfield(FIELD_CAR, command_name);
    else if (strcmp(command_name, "setcdr") == 0)
        cmd_setfield(FIELD_CDR, command_name);
    else if (strcmp(command_name, "settag") == 0)
        cmd_settag();
    else if (strcmp(command_name, "setatom") == 0)
        cmd_setatom();
    else
        unknown_command(command_name);

    return;
}



// Individual commands:

void cmd_getfield(int field, const char *command_name) {
    int index;

    if (!get_int_argument_strtok(command_name, &index)) return;
    if (!no_more_arguments_strtok(command_name)) return;

    if (index < 0 || index >= HEAP_SIZE) {
        index_out_of_range(index);
        return;
    }

    int result = getfield(heap, field, index);

    printf("%d\n", result);
}

void cmd_setfield(int field, const char *command_name) {
    int index, value;

    if (!get_int_argument_strtok(command_name, &index)) return;
    if (!get_int_argument_strtok(command_name, &value)) return;
    if (!no_more_arguments_strtok(command_name)) return;

    if (index < 0 || index >= HEAP_SIZE) {
        index_out_of_range(index);
        return;
    }

    setfield(heap, field, index, value);
}

void cmd_gettag() {
    int index;
    const char *command_name = "gettag";

    if (!get_int_argument_strtok(command_name, &index)) return;
    if (!no_more_arguments_strtok(command_name)) return;

    if (index < 0 || index >= HEAP_SIZE) {
        index_out_of_range(index);
        return;
    }

    int result = getfield(heap, FIELD_TAG, index);

    switch (result) {
        case TAG_UNINIT:
            printf("uninit\n");
            return;
        case TAG_ATOM:
            printf("atom\n");
            return;
        case TAG_CONS:
            printf("cons\n");
            return;
        default:
            PANIC("Unrecognized tag number: %d", result);
    }
}

void cmd_settag() {
    int index, value;
    const char *command_name = "settag";

    if (!get_int_argument_strtok(command_name, &index)) return;
    if (!get_tagname_argument_strtok(command_name, &value)) return;
    if (!no_more_arguments_strtok(command_name)) return;

    if (index < 0 || index >= HEAP_SIZE) {
        index_out_of_range(index);
        return;
    }

    setfield(heap, FIELD_TAG, index, value);
}

void cmd_getatom() {
    int index;
    const char *command_name = "getatom";

    if (!get_int_argument_strtok(command_name, &index)) return;
    if (!no_more_arguments_strtok(command_name)) return;

    if (index < 0 || index >= HEAP_SIZE) {
        index_out_of_range(index);
        return;
    }

    if (!isatom(heap, index)) {
        not_an_atom(index);
        return;
    }

    const char *text = getatom(heap, index);

    printf("%s\n", text);
}

void cmd_setatom() {
    int index;
    const char *text;
    const char *command_name = "setatom";

    if (!get_int_argument_strtok(command_name, &index)) return;
    if (!get_word_argument_strtok(command_name, &text)) return;
    if (!no_more_arguments_strtok(command_name)) return;

    if (index < 0 || index >= HEAP_SIZE) {
        index_out_of_range(index);
        return;
    }

    setatom(heap, index, text);
}



// Error messages:

void unknown_command(const char *command_name) {
    fprintf(stderr, "Unrecognized command: %s\n", command_name);
}

void too_few_arguments(const char *command_name) {
    fprintf(stderr, "Too few arguments to %s\n", command_name);
}

void too_many_arguments(const char *command_name) {
    fprintf(stderr, "Too many arguments to %s\n", command_name);
}

void invalid_number(const char *word) {
    fprintf(stderr, "Invalid number: %s\n", word);
}

void index_out_of_range(int index) {
    fprintf(stderr, "Index out of range: %d\n", index);
}

void unknown_tagname(const char *word) {
    fprintf(stderr,  "Unrecognized tag name: %s\n", word);
}

void not_an_atom(int index) {
    fprintf(stderr, "The cell at index %d is not a valid atom\n", index);
}



// Argument parsing using strtok:

int get_int_argument_strtok(const char *command_name, int *result) {
    const char *word = strtok(NULL, " \n");

    if (!word) {
        too_few_arguments(command_name);
        return 0;
    }

    char *remainder;
    int strtoul_result = strtoul(word, &remainder, 10);

    if (remainder == word) {
        invalid_number(word);
        return 0;
    } else {
        *result = strtoul_result;
        return 1;
    }
}

int get_word_argument_strtok(const char *command_name, const char **result) {
    const char *word = strtok(NULL, " \n");

    if (!word) {
        too_few_arguments(command_name);
        return 0;
    }

    *result = word;
    return 1;
}

int get_tagname_argument_strtok(const char *command_name, int *result) {
    const char *word = strtok(NULL, " \n");

    if (!word) {
        too_few_arguments(command_name);
        return 0;
    }

    if (strcmp(word, "uninit") == 0) {
        *result = TAG_UNINIT;
        return 1;
    } else if (strcmp(word, "atom") == 0) {
        *result = TAG_ATOM;
        return 1;
    } else if (strcmp(word, "cons") == 0) {
        *result = TAG_CONS;
        return 1;
    } else {
        unknown_tagname(word);
        return 0;
    }
}

int no_more_arguments_strtok(const char *command_name) {
    const char *word = strtok(NULL, " \n");

    if (word) {
        too_many_arguments(command_name);
        return 0;
    } else {
        return 1;
    }
}
