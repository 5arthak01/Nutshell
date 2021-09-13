#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 4096

int main()
{
    size_t input_len = MAX_INPUT_SIZE;
    char *input = malloc(sizeof(char) * input_len);

    while (1)
    {
        if (getline(&input, &input_len, stdin) == -1)
        {
            fprintf(stderr, "Error while getting input.\n");
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Error in forking");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // inside child
            printf("You entered %s\n", input);

            exit(EXIT_SUCCESS);

            // execvp()
        }
        else
        {
            // inside parent, now pid is of child
            int status;
            waitpid(pid, &status, 0);

            if (!WIFEXITED(status))
            {
                fprintf(stderr, "An Error occured.");
            }
            // printf("Inside parent\n");
        }
    }
    free(input);

    return 0;
}