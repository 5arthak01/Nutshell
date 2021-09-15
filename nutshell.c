#include "includes.h"
#include "constants.h"
#include "prompt.h"
#include "utils.h"

int main()
{
    char *input;
    char *SHELL_HOME_PATH = getcwd(NULL, 0);
    char **tokens = malloc(sizeof(char *) * MAX_ARGS_COUNT);

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

            tokenise(input, WHITESPACE_TOKENS, tokens);

            for (int i = 0;; i++)
            {
                if (tokens[i])
                {
                    printf(" %s\n", tokens[i]);
                }
                else
                {
                    break;
                }
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
    free(tokens);

    return 0;
}