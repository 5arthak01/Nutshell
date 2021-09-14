#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include <sys/utsname.h>

#define MAX_INPUT_SIZE 4096
#define MAX_HOSTNAME_SIZE 256 // according to SUSv2
#define TOKEN_WHITESPACE " \t"

void replace_home_with_tilde(char *path, char *home_path)
{
    /*
    Replaces the absolute path of the 
    home directory with a '~' in `path`
    */

    int len_path = strlen(path);
    int len_home_path = strlen(home_path);

    if (len_path >= len_home_path && strncmp(home_path, path, len_home_path) == 0)
    {
        // path contains home, so replacement
        char *new_path = malloc(sizeof(char) * (len_path - len_home_path + 5));
        new_path[0] = '~';
        strcpy(new_path + 1, path + len_home_path);
        strcpy(path, new_path);
        free(new_path);
    }
}

void print_prompt(char *home_path)
{
    /*
    Prints shell prompt 
    */

    //get username
    char hostname[MAX_HOSTNAME_SIZE];
    if (gethostname(hostname, MAX_HOSTNAME_SIZE) == -1)
    {
        perror("Error when getting hostname");
        exit(EXIT_FAILURE);
    }

    // get system name
    struct utsname buf;
    if (uname(&buf) == -1)
    {
        perror("Error in buffer while trying to retrieve system name");
        exit(EXIT_FAILURE);
    }

    // get path
    char *pwd_path = getcwd(NULL, 0);
    replace_home_with_tilde(pwd_path, home_path);

    printf("<%s@%s:%s> ", hostname, buf.sysname, pwd_path);
    fflush(stdout);
    free(pwd_path);
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
    char *SHELL_HOME_PATH = getcwd(NULL, 0);

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
            print_prompt(SHELL_HOME_PATH);
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
    free(SHELL_HOME_PATH);

    return 0;
}