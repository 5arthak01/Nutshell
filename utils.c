#include "includes.h"
#include "constants.h"

char *read_input()
{
    ssize_t input_len = MAX_INPUT_SIZE;
    char *input = malloc(sizeof(char) * input_len);
    if (getline(&input, &input_len, stdin) == -1)
    {
        if (!feof(stdin))
        {
            // getline not at EOF, so error
            perror("Error while taking input");
            exit(EXIT_FAILURE);
        }

        // if (feof(stdin))
        // {
        //     // EOF reached
        //     exit(EXIT_SUCCESS);
        // }
        // else
        // {
        //     perror("Error while taking input");
        //     exit(EXIT_FAILURE);
        // }
    }
    return input;
}

void trim(char *str)
{
    /* 
    trims leading and trailing whitespaces
    modifies given string
    */

    int len = strlen(str);
    int cur; // iterator/cursor
    for (cur = len - 1; cur >= 0; cur--)
    {
        if (isspace(str[cur]))
        {
            break;
        }
    }
    if (cur != -1)
    {
        // trailing spaces were present
        str[cur] = '\0';
    }

    len = strlen(str);

    for (cur = 0; cur < len; cur++)
    {
        if (isspace(str[cur]))
        {
            break;
        }
    }

    if (cur != len)
    {
        // leading spaces were present
        memmove(str, str + cur, strlen(str));
    }
}
