
#include "includes.h"
#include "constants.h"
#include "types.h"
#include "utils.h"
#include "builtins.h"

#include <signal.h>

void handle_bg_terminate(int sig, siginfo_t *info, void *ucontext)
{
    // get process name from pid
    // NOTE: There does not exist a portable way of doing this, this
    // implementation is limited to Linux (or some UNIX implementations)
    pid_t pid = info->si_pid;
    char proc_file[MAX_PATH_LEN];
    char proc_name[MAX_PATH_LEN];
    sprintf(proc_file, "/proc/%d/cmdline", pid);
    FILE *f = fopen(proc_file, "r");
    if (f)
    {
        size_t file_size;
        file_size = fread(proc_name, sizeof(char), sizeof(proc_file), f);
        if (file_size > 0)
        {
            if ('\n' == proc_name[file_size - 1])
                proc_name[file_size - 1] = '\0';
        }
        fclose(f);
    }

    // Reap the process
    int wstatus;
    pid = waitpid(pid, &wstatus, WNOHANG);
    if (pid > 0)
    {
        // assumes info->si_code == CLD_EXITED
        fprintf(stderr, "\n%s with pid %d exited %s\n", proc_name, pid, info->si_status ? "abnormally" : "normally");
    }
    // else some error occured
}

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

            int bg_process = cmd.args[cmd.num_args - 1][0] == '&';

            if (pid < 0)
            {
                perror("Error in forking");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                // place child in new group
                setpgid(0, 0);
                if (execvp(cmd.args[0], cmd.args) == -1)
                {
                    fprintf(stderr, "'%s': command not found\n", cmd.args[0]);
                    exit(EXIT_FAILURE);
                }

                exit(EXIT_SUCCESS);
            }
            else
            {
                // inside parent, now pid is of child

                if (bg_process)
                {
                    // print the PID of the newly created bg process
                    printf("%d\n", pid);
                    struct sigaction sa;
                    sa.sa_sigaction = &handle_bg_terminate;
                    sa.sa_flags = SA_NOCLDSTOP | SA_RESTART | SA_SIGINFO;
                    sigaction(SIGCHLD, &sa, NULL);
                }
                else
                {
                    // ignore terminal signals
                    // for I/O by child
                    sigaction(SIGTTIN, SIG_IGN, NULL);
                    sigaction(SIGTTOU, SIG_IGN, NULL);

                    // set terminal foreground process group
                    // as the pid of the child
                    // (since we made child pgrp = pid)
                    tcsetpgrp(STDIN_FILENO, pid);

                    // wait for child
                    int wstatus;
                    waitpid(pid, &wstatus, WUNTRACED);

                    // restore terminal control
                    sigaction(SIGTTIN, SIG_DFL, NULL);
                    sigaction(SIGTTOU, SIG_DFL, NULL);
                    tcsetpgrp(STDIN_FILENO, getpgrp());
                }
            }
        }
    }
}