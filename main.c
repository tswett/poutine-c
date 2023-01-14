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



// Command parsing and processing:

// Read a command from the user and run it
void process_command(void);

// Individual commands:

// Get and print the value of a field in a cell
void cmd_getfield(int field, char *command_name);
// Set the value of a field in a cell
void cmd_setfield(int field, char *command_name);

// Error messages:

// Print "Unrecognized command: %s"
void unknown_command(char *command_name);
// Print "Too few arguments to %s"
void too_few_arguments(char *command_name);
// Print "Too many arguments to %s"
void too_many_arguments(char *command_name);
// Print "Invalid number: %s"
void invalid_number(char *word);
// Print "Index out of range: %d"
void index_out_of_range(int index);

// Argument parsing using strtok:

// Try to get an int argument; return 0 on failure
int get_int_argument_strtok(char *command_name, int *result);
// Assert that there are no more arguments; return 0 if there is one
int no_more_arguments_strtok(char *command_name);



// Command parsing and processing:

int main(int argc, char **argv) {
    while (!feof(stdin)) {
        process_command();
    }
}

void process_command(void) {
    char command[1024];

    fprintf(stderr, "> ");
    if (!fgets(command, sizeof(command), stdin))
        return;

    char *command_name = strtok(command, " \n");

    if (!command_name)
        return;

    if (strcmp(command_name, "getcar") == 0)
        cmd_getfield(FIELD_CAR, command_name);
    else if (strcmp(command_name, "getcdr") == 0)
        cmd_getfield(FIELD_CDR, command_name);
    else if (strcmp(command_name, "setcar") == 0)
        cmd_setfield(FIELD_CAR, command_name);
    else if (strcmp(command_name, "setcdr") == 0)
        cmd_setfield(FIELD_CDR, command_name);
    else
        unknown_command(command_name);

    return;
}



// Individual commands:

void cmd_getfield(int field, char *command_name) {
    int index;

    if (!get_int_argument_strtok(command_name, &index)) return;
    if (!no_more_arguments_strtok(command_name)) return;

    if (index < 0 || index >= HEAP_SIZE) {
        index_out_of_range(index);
        return;
    }

    int result = getfield(field, index);

    printf("%d\n", result);
}

void cmd_setfield(int field, char *command_name) {
    int index, value;

    if (!get_int_argument_strtok(command_name, &index)) return;
    if (!get_int_argument_strtok(command_name, &value)) return;
    if (!no_more_arguments_strtok(command_name)) return;

    if (index < 0 || index >= HEAP_SIZE) {
        index_out_of_range(index);
        return;
    }

    setfield(field, index, value);
}



// Error messages:

void unknown_command(char *command_name) {
    fprintf(stderr, "Unrecognized command: %s\n", command_name);
}

void too_few_arguments(char *command_name) {
    fprintf(stderr, "Too few arguments to %s\n", command_name);
}

void too_many_arguments(char *command_name) {
    fprintf(stderr, "Too many arguments to %s\n", command_name);
}

void invalid_number(char *word) {
    fprintf(stderr, "Invalid number: %s\n", word);
}

void index_out_of_range(int index) {
    fprintf(stderr, "Index out of range: %d\n", index);
}



// Argument parsing using strtok:

int get_int_argument_strtok(char *command_name, int *result) {
    char *word = strtok(NULL, " \n");

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

int no_more_arguments_strtok(char *command_name) {
    char *word = strtok(NULL, " \n");

    if (word) {
        too_many_arguments(command_name);
        return 0;
    } else {
        return 1;
    }
}
