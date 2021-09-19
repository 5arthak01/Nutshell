#include "includes.h"
#include "constants.h"
#include "types.h"

#include <sys/stat.h>
#include <errno.h>

extern int errno;

void pinfo(command cmd)
{
    char pid_dir[MAX_PATH_LEN];
    switch (cmd.num_args)
    {
    case 1:
        sprintf(pid_dir, "/proc/%d", getpid());
        break;
    case 2:
        sprintf(pid_dir, "/proc/%s", cmd.args[1]);
        break;
    default:
        fprintf(stderr, "\nError: Too many arguments\nUsage: pinfo <pid>\n");
        return;
    }

    struct stat s;
    int err = stat(pid_dir, &s);
    if (err == -1)
    {
        if (ENOENT == errno)
        {
            fprintf(stderr, "\nPlease provide valid PID.\n");
            return;
        }
        else
        {
            perror("stat");
            exit(EXIT_FAILURE);
        }
    }
    else if (!S_ISDIR(s.st_mode))
    {
        fprintf(stderr, "\nSomething went wrong.\n");
        return;
    }

    char pid_file_path[MAX_PATH_LEN + 10];
    sprintf(pid_file_path, "%s/stat", pid_dir);
    FILE *stat_file = fopen(pid_file_path, "r");
    if (stat_file)
    {
        int pid;
        char status;
        fscanf(stat_file, "%d %*s %c", &pid, &status);

        printf("pid -- %d\n", pid);
        printf("Process Status -- %c\n", status);
        fclose(stat_file);
    }
    else
    {
        perror("Error when getting process status");
        return;
    }

    sprintf(pid_file_path, "%s/statm", pid_dir);
    FILE *memory_file = fopen(pid_file_path, "r");
    if (memory_file)
    {
        int proc_size;
        fscanf(memory_file, "%d", &proc_size);

        printf("memory -- %d\n", proc_size);
        fclose(memory_file);
    }
    else
    {
        perror("Error when getting process memory");
        return;
    }

    sprintf(pid_file_path, "%s/exe", pid_dir);
    char executable_path[MAX_PATH_LEN];
    // readlink does not return null terminated
    // buffer due to portability reasons

    ssize_t num_buf_bytes = readlink(pid_file_path, executable_path, MAX_PATH_LEN);
    if (num_buf_bytes < 0)
    {
        perror("Error when getting process executable path");
        return;
    }
    executable_path[num_buf_bytes] = '\0';
    printf("Executable Path -- %s\n", executable_path);
}
