#include "includes.h"
#include "constants.h"
#include "types.h"
#include "cd.h"
#include "getcwd.h"
#include "echo.h"
#include "ls.h"

// refer NUM_BUILTINS in constants.h for size
char *builtin_cmd_names[] = {
    "cd",
    "pwd",
    "echo",
    "ls"};

void (*builtin_cmds[])(command) = {
    &cd,
    &pwd,
    &echo,
    &ls};

int get_builtin_id(char *func_name)
{
    for (int i = 0; i < NUM_BUILTINS; i++)
    {
        if (strcmp(func_name, builtin_cmd_names[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}
