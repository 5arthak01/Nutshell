#include "includes.h"
#include "constants.h"
#include "prompt.h"
#include "utils.h"
#include "execution.h"

int main()
{
    char *input = malloc(sizeof(char) * MAX_INPUT_SIZE);
    char *SHELL_HOME_PATH = getcwd(NULL, 0);
    int num_tokens;

    while (1)
    {
        print_prompt(SHELL_HOME_PATH);

        read_input(input);

        char *command = strtok(input, ";");
        while (command)
        {
            execute(command);
            // go to next command
            command = strtok(NULL, ";");
        }
    }

    free(input);
    free(SHELL_HOME_PATH);

    return 0;
}