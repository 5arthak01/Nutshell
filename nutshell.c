#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT_SIZE 4096
#define TOKEN_WHITESPACE " \t"

void print_prompt()
{
    char *cwd = getcwd(NULL, 0);

    printf("<username@system_name:%s> ", cwd);
    fflush(stdout);
    free(cwd);
}

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

int main()
{
    char *input;

    while (1)
    {

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Error in forking");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // inside child
            print_prompt();
            input = read_input();

            printf("You entered %s\n", input);

            char *token = strtok(input, TOKEN_WHITESPACE);
            while (token)
            {
                printf("%s\n", token);

                token = strtok(NULL, TOKEN_WHITESPACE);
            }

            exit(EXIT_SUCCESS);

            // execvp()
        }
        else
        {
            // inside parent, now pid is of child
            int wstatus;
            waitpid(pid, &wstatus, 0);

            if (!WIFEXITED(wstatus))
            {
                // include signal handling for SIGTSTP?
                fprintf(stderr, "An Error occured.");
            }
            // printf("Inside parent\n");
        }
    }
    free(input);

    return 0;
}