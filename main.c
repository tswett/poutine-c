// Copyright 2022 by Tanner Swett and Medallion Instrumentation Systems.
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
// Try to parse an int from a string; return 0 on failure
int try_parse_int(char *input, int *result);

// Individual commands:

// Get and print the value of a field in a cell
void cmd_getfield(int field, int index);
// Set the value of a field in a cell
void cmd_setfield(int field, int index, int value);
#define COMMAND_GETCAR 0
#define COMMAND_GETCDR 1
#define COMMAND_SETCAR 2
#define COMMAND_SETCDR 3

// Command parsing and processing:

int main(int argc, char **argv) {
    while (!feof(stdin)) {
        process_command();
    }
}

void process_command(void) {
    char command[1024];
    char *word;
    char *command_name;
    int command_num;

    fprintf(stderr, "> ");
    if (!fgets(command, sizeof(command), stdin))
        return;

    command_name = strtok(command, " \n");

    if (!command_name)
        return;

    if (strcmp(command_name, "getcar") == 0)
        command_num = COMMAND_GETCAR;
    else if (strcmp(command_name, "getcdr") == 0)
        command_num = COMMAND_GETCDR;
    else if (strcmp(command_name, "setcar") == 0)
        command_num = COMMAND_SETCAR;
    else if (strcmp(command_name, "setcdr") == 0)
        command_num = COMMAND_SETCDR;
    else
        goto unknown_command;

    switch (command_num) {
        case COMMAND_GETCAR:
        case COMMAND_GETCDR: {
            int index;

            word = strtok(NULL, " \n");
            if (!word) goto too_few_arguments;
            if (!try_parse_int(word, &index)) goto invalid_number;

            word = strtok(NULL, " \n");
            if (word) goto too_many_arguments;

            switch (command_num) {
                case COMMAND_GETCAR:
                    cmd_getfield(FIELD_CAR, index);
                    break;
                case COMMAND_GETCDR:
                    cmd_getfield(FIELD_CDR, index);
                    break;
            }

            break;
        }

        case COMMAND_SETCAR:
        case COMMAND_SETCDR: {
            int index, value;

            word = strtok(NULL, " \n");
            if (!word) goto too_few_arguments;
            if (!try_parse_int(word, &index)) goto invalid_number;

            word = strtok(NULL, " \n");
            if (!word) goto too_few_arguments;
            if (!try_parse_int(word, &value)) goto invalid_number;

            word = strtok(NULL, " \n");
            if (word) goto too_many_arguments;

            switch (command_num) {
                case COMMAND_SETCAR:
                    cmd_setfield(FIELD_CAR, index, value);
                    break;
                case COMMAND_SETCDR:
                    cmd_setfield(FIELD_CDR, index, value);
                    break;
            }

            break;
        }
    }

    return;

    unknown_command:
    fprintf(stderr, "Unrecognized command: %s\n", command_name);
    return;

    too_few_arguments:
    fprintf(stderr, "Too few arguments to %s\n", command_name);
    return;

    too_many_arguments:
    fprintf(stderr, "Too many arguments to %s\n", command_name);
    return;

    invalid_number:
    fprintf(stderr, "Invalid number: %s\n", word);
    return;
}

int try_parse_int(char *input, int *result) {
    char *remainder;
    int strtoul_result = strtoul(input, &remainder, 10);

    if (remainder == input) {
        return 0;
    } else {
        *result = strtoul_result;
        return 1;
    }
}

// Individual commands:

void cmd_getfield(int field, int index) {
    int result;

    if (index < 0 || index >= HEAP_SIZE) {
        fprintf(stderr, "Index out of range: %d\n", index);
        return;
    }

    result = getfield(field, index);

    printf("%d\n", result);
}

void cmd_setfield(int field, int index, int value) {
    if (index < 0 || index >= HEAP_SIZE) {
        fprintf(stderr, "Index out of range: %d\n", index);
        return;
    }

    setfield(field, index, value);
}
