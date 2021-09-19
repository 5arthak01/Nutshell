#include "includes.h"
#include "constants.h"
#include "types.h"

#include <errno.h>

extern int errno;

#include "execution.h"

void repeat(command cmd)
{
    if (cmd.num_args <= 2)
    {
        fprintf(stderr, "\nInsufficient arguments.\nUsage: repeat <n> <command>\n");
        return;
    }

    long long int n = strtoll(cmd.args[1], NULL, 10);
    if (n < 0 || errno == EINVAL)
    {
        fprintf(stderr, "\nPlease enter a valid number\n");
        return;
    }
    else if (n == 0)
    {
        return;
    }
    else if (errno == ERANGE)
    {
        fprintf(stderr, "\nNumber too large.\n");
        return;
    }

    for (int i = 2; i < cmd.num_args; i++)
    {
        cmd.args[i - 2] = cmd.args[i];
    }
    cmd.num_args -= 2;

    while (n--)
    {
        execute_tokenised_cmd(cmd);
    }
}