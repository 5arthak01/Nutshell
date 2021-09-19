#include "includes.h"
#include "constants.h"
#include "types.h"
#include "prompt.h"
#include "utils.h"
#include "execution.h"

int main()
{
    char *input;
    size_t input_len;
    getcwd(SHELL_HOME_PATH, MAX_PATH_LEN);

    while (1)
    {
        print_prompt();

        // read input
        input = NULL;
        input_len = 0;
        if (getline(&input, &input_len, stdin) == -1)
        {
            if (!feof(stdin))
            {
                // getline not at EOF, so other error
                perror("Error while taking input");
                free(input);
                exit(EXIT_FAILURE);
            }
        }
        input[strlen(input) - 1] = '\0';

        // if non-null input received
        if (input)
        {
            char *command = strtok(input, ";");
            while (command)
            {
                execute(command);
                // go to next command
                command = strtok(NULL, ";");
            }
        }
    }

    free(input);

    return 0;
}