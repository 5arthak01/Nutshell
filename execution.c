
#include "includes.h"
#include "constants.h"
#include "types.h"
#include "utils.h"
#include "builtins.h"

void execute(char *input_cmd, char *shell_home_path)
{
    trim(input_cmd);

    command cmd;

    tokenise(input_cmd, WHITESPACE_TOKENS, cmd.args, &cmd.num_args);

    if (cmd.num_args > 0)
    {
        int id = get_builtin_id(cmd.args[0]);

        if (id != -1)
        {
            if (strcmp("cd", cmd.args[0]) == 0 || strcmp("ls", cmd.args[0]) == 0)
            {
                cmd.internal_args = shell_home_path;
            }
            builtin_cmds[id](cmd);
        }
        else
        {
            pid_t pid = fork();

            if (pid < 0)
            {
                perror("Error in forking");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                if (execvp(cmd.args[0], cmd.args) == -1)
                {
                    perror("Error in executing command");
                    exit(EXIT_FAILURE);
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
    }
}