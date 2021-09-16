#include "includes.h"
#include "constants.h"
#include "types.h"

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
        if (!isspace(str[cur]))
        {
            break;
        }
    }
    if (cur != -1 && cur != len - 1)
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
    Tokenises the string according to given `delim`
    and stores them in array of strings `tokens`.
    */
    int i = 0;
    if (str != NULL)
    {
        char *token = strtok(str, delim);
        while (token)
        {
            tokens[i] = token;
            i++;
            token = strtok(NULL, delim);
        }
    }

    tokens[i] = NULL;
    *num_tokens = i;
}

/*
// DEPRECATED
void read_input(char **input)
{
    *input = NULL;
    size_t input_len = 0;

    if (getline(input, &input_len, stdin) == -1)
    {
        if (!feof(stdin))
        {
            // getline not at EOF, so other error
            perror("Error while taking input");
            exit(EXIT_FAILURE);
        }
    }
    *input[strlen(*input) - 1] = '\0';
}
*/