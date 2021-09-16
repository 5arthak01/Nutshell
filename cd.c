#include "includes.h"
#include "types.h"
#include "utils.h"
#include "constants.h"
#include "getcwd.h"

// variable name same as BASH
// working directory originally home
char oldpwd[MAX_PATH_LEN] = "~";

void cd(command cmd)
{
    int prev_dir_flag = 0;
    char new_path[MAX_PATH_LEN];
    if (cmd.num_args == 1)
    {
        // no path given, so
        // go to `shell_home_path`
        strcpy(new_path, cmd.internal_args);
    }
    else if (cmd.num_args == 2)
    {
        if (strcmp(cmd.args[1], "-") == 0)
        {
            // go to previous working directory
            strcpy(new_path, oldpwd);
            // set flag to print later
            prev_dir_flag = 1;
        }
        else
        {
            strcpy(new_path, cmd.args[1]);
        }

        replace_tilde_with_home(new_path, cmd.internal_args);
    }
    else
    {
        fprintf(stderr, "Usage: cd <path>\n");
        exit(EXIT_FAILURE);
        return;
    }

    // store absolute path of present directory
    char *cwd = getcwd(NULL, 0);

    // change into new directory
    if (chdir(new_path) != 0)
    {
        printf("%s\n", new_path);
        perror("Error while changing directory");
        free(cwd);
        return;
    }
    else
    {
        // store previous directory path
        strcpy(oldpwd, cwd);
    }

    if (prev_dir_flag)
    {
        // print absolute pathname of new
        // directory since "-" was used
        pwd(cmd);
    }

    free(cwd);
}