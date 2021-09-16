#include "includes.h"
#include "constants.h"
#include "types.h"
#include "cd.h"
#include "getcwd.h"
#include "echo.h"

// refer NUM_BUILTINS in constants.h for size
char *builtin_cmd_names[] = {
    "cd",
    "pwd",
    "echo"};

void (*builtin_cmds[])(command) = {
    &cd,
    &pwd,
    &echo};

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
