#include "includes.h"
#include "constants.h"
#include <pwd.h>

void replace_home_with_tilde(char *path)
{
    /*
    Replaces the absolute path of the 
    home directory with a '~' in `path`
    */

    // Not using strstr() since
    // lengths are useful inside
    // so saves O(m+n) computation

    int len_path = strlen(path);
    int len_home_path = strlen(SHELL_HOME_PATH);
    if (len_path >= len_home_path && strncmp(SHELL_HOME_PATH, path, len_home_path) == 0)
    {
        // path contains home, so replacement
        char *new_path = malloc(sizeof(char) * (len_path - len_home_path + 5));
        new_path[0] = '~';
        strcpy(new_path + 1, path + len_home_path);
        strcpy(path, new_path);
        free(new_path);
    }
}

void print_prompt()
{
    /*
    Prints shell prompt 
    */

    //get username
    char *username = getpwuid(getuid())->pw_name;

    // get system name
    char hostname[MAX_HOSTNAME_SIZE];
    if (gethostname(hostname, MAX_HOSTNAME_SIZE) == -1)
    {
        perror("Error when getting hostname");
        exit(EXIT_FAILURE);
    }
    // note:
    // uname() can be used too, but the implementation
    // on Linux just calls hostname under the hood

    // get path
    char *pwd_path = getcwd(NULL, 0);
    replace_home_with_tilde(pwd_path);

    printf("<%s@%s:%s> ", username, hostname, pwd_path);
    fflush(stdout);
    free(pwd_path);
}
