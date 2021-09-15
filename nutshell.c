#include "includes.h"
#include "constants.h"
#include "prompt.h"
#include "utils.h"

int main()
{
    char *input = malloc(sizeof(char) * MAX_INPUT_SIZE);
    char *SHELL_HOME_PATH = getcwd(NULL, 0);
    char **tokenised_input = malloc(sizeof(char *) * MAX_ARGS_COUNT);
    int num_tokens;

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

            read_input(input);
            trim(input);
            tokenise(input, WHITESPACE_TOKENS, tokenised_input, &num_tokens);

            if (execvp(tokenised_input[0], tokenised_input) == -1)
            {
                perror("Error ");
                exit(EXIT_FAILURE);
            }

            exit(EXIT_SUCCESS);
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
    free(tokenised_input);

    return 0;
}