// desc:
// parse passed command line arguments argv[] and argc
// and call the appropriate function
// based on the command line arguments
// passed to the program
//
// Author: dvidakk
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"

typedef struct {
    char *command;
    void (*function)(int argc, char *argv[]);
} Command;

typedef struct {
    char short_name;
    char *long_name;
    char *arg_type;
    void *value;
} Arg;

// create a struct to represent a command with a name and a description and a list of arguments
// that the command can take
//

// define a list of arguments that the program can take
Arg args[] = {
    {'h', "help", NULL, NULL},
    {'d', "depth", "int", 1},
    {'r', "room", "string", NULL},
};
