#ifndef __BUILTINS_H
#define __BUILTINS_H

#include "constants.h"

int get_builtin_id(char *func_name);

void (*builtin_cmds[NUM_BUILTINS])(command);

#endif