#ifndef __TYPES_H
#define __TYPES_H

#include "includes.h"
#include "constants.h"

typedef struct command
{
    char *args[MAX_ARGS_COUNT];
    int num_args;
    // char *internal_args;
} command;

#endif