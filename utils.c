#include "includes.h"
#include "constants.h"

void read_input(char *input)
{
    ssize_t input_len = MAX_INPUT_SIZE;
    if (getline(&input, &input_len, stdin) == -1)
    {
        if (!feof(stdin))
        {
            // getline not at EOF, so other error
            perror("Error while taking input");
            exit(EXIT_FAILURE);
        }
    }
}

void trim(char *str)
{
    /* 
    Trims leading and trailing whitespaces,
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
        if (!isspace(str[cur]))
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

void tokenise(char *str, char *delim, char **tokens, int *num_tokens)
{
    /*
    Tokenises the string according to given delim
    and returns new array of string tokens.
    */
    char *token = strtok(str, delim);
    int i = 0;
    while (token)
    {
        tokens[i] = token;
        i++;
        token = strtok(NULL, delim);
    }

    tokens[i] = NULL;
    *num_tokens = i;
}