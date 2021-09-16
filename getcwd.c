#include "includes.h"
#include "constants.h"
#include "types.h"

void pwd(command cmd)
{
    char *cwd = getcwd(NULL, 0);
    printf("%s\n", cwd);
    free(cwd);
}