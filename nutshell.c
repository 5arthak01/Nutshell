#include "includes.h"
#include "constants.h"
#include "prompt.h"
#include "utils.h"
#include "execution.h"

int main()
{
    char *input;
    size_t input_len;
    char *SHELL_HOME_PATH = getcwd(NULL, 0);

    while (1)
    {
        print_prompt(SHELL_HOME_PATH);
        input = NULL;
        input_len = 0;
        if (getline(&input, &input_len, stdin) == -1)
        {
            if (!feof(stdin))
            {
                // getline not at EOF, so other error
                perror("Error while taking input");
                exit(EXIT_FAILURE);
            }
        }
        input[strlen(input) - 1] = '\0';

        // read_input(&input);
        if (input)
        {
            char *command = strtok(input, ";");
            while (command)
            {
                execute(command, SHELL_HOME_PATH);
                // go to next command
                command = strtok(NULL, ";");
            }
        }
    }

    free(input);
    free(SHELL_HOME_PATH);

    return 0;
}