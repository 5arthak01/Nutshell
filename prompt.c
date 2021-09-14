#include "includes.h"
#include "constants.h"
#include <sys/utsname.h>

void replace_home_with_tilde(char *path, char *home_path)
{
    /*
    Replaces the absolute path of the 
    home directory with a '~' in `path`
    */

    int len_path = strlen(path);
    int len_home_path = strlen(home_path);

    if (len_path >= len_home_path && strncmp(home_path, path, len_home_path) == 0)
    {
        // path contains home, so replacement
        char *new_path = malloc(sizeof(char) * (len_path - len_home_path + 5));
        new_path[0] = '~';
        strcpy(new_path + 1, path + len_home_path);
        strcpy(path, new_path);
        free(new_path);
    }
}

void print_prompt(char *home_path)
{
    /*
    Prints shell prompt 
    */

    //get username
    char hostname[MAX_HOSTNAME_SIZE];
    if (gethostname(hostname, MAX_HOSTNAME_SIZE) == -1)
    {
        perror("Error when getting hostname");
        exit(EXIT_FAILURE);
    }

    // get system name
    struct utsname sysinfo;
    if (uname(&sysinfo) == -1)
    {
        perror("Error in buffer while trying to retrieve system name");
        exit(EXIT_FAILURE);
    }

    // get path
    char *pwd_path = getcwd(NULL, 0);
    replace_home_with_tilde(pwd_path, home_path);

    printf("<%s@%s:%s> ", hostname, sysinfo.sysname, pwd_path);
    fflush(stdout);
    free(pwd_path);
}
