#ifndef __BUILTINS_H
#define __BUILTINS_H

int get_builtin_id(char *func_name);

void (*builtin_cmds[])(char **);

#endif