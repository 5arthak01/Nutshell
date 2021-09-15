
#include "includes.h"
#include "constants.h"
#include "utils.h"

void execute(char *command)
{

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Error in forking");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        trim(command);
        char *tokenised_command[MAX_ARGS_COUNT];
        int num_args = 0;

        tokenise(command, WHITESPACE_TOKENS, tokenised_command, &num_args);

        if (num_args)
        {

            if (execvp(tokenised_command[0], tokenised_command) == -1)
            {
                perror("Error in executing command");
                exit(EXIT_FAILURE);
            }
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        // inside parent, now pid is of child
        int wstatus;
        waitpid(pid, &wstatus, WUNTRACED);

        if (!WIFEXITED(wstatus))
        {
            // include signal handling for SIGTSTP?
            fprintf(stderr, "An Error occured.\n");
        }
        // printf("Inside parent\n");
    }
}