#include "includes.h"
#include "constants.h"

char OLDPWD[MAX_PATH_LEN] = "~"; // originally at home

void cd(char **command)
{
    int i = 0;
    while (command[i])
    {
        i++;
    }

    if (i == 1)
    {
        chdir(command[1]);
    }
    else if (i == 2)
    {
        if (chdir(command[1]) != 0)
        {
            perror("Error while changing directory");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fprintf(stderr, "Usage: cd <path>\n");
        exit(EXIT_FAILURE);
        return;
    }
}