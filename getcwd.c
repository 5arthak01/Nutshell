#include "includes.h"
#include "constants.h"

void pwd(char **command)
{
    char *cwd = getcwd(NULL, 0);
    printf("%s\n", cwd);
    free(cwd);
}