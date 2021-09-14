#include "includes.h"
#include "constants.h"
#include "prompt.h"
#include "utils.h"

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