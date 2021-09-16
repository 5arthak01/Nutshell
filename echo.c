#include "includes.h"
#include "constants.h"
#include "types.h"

void echo(command cmd)
{
    // print all tokenised arguments
    // except the command name `echo`
    for (int i = 1; i < cmd.num_args; i++)
    {
        printf("%s", cmd.args[i]);
    }
    printf("\n");
}