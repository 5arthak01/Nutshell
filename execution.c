
#include "includes.h"
#include "constants.h"
#include "types.h"
#include "utils.h"
#include "builtins.h"
#include "prompt.h"

#include <signal.h>

void handle_bg_terminate(int sig, siginfo_t *info, void *ucontext)
{
    // The child has terminated but not yet reaped
    // So we first get the name of the process
    // and then reap it

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
        fflush(stderr);
        // fflush(stdout);
        print_prompt();
    }
    // else some error occured with waitpid
    // or child(ren) still exist
}

void execute(char *input_cmd)
{
    trim(input_cmd);

    command cmd;
    tokenise(input_cmd, WHITESPACE_TOKENS, cmd.args, &cmd.num_args);

    if (cmd.num_args > 0)
    {
        int id = get_builtin_id(cmd.args[0]);

        if (id != -1)
        {
            builtin_cmds[id](cmd);
        }
        else
        {
            pid_t pid = fork();

            // assume fg process
            int bg_process = 0;
            if (cmd.args[cmd.num_args - 1][0] == '&')
            {
                // set flag and remove trailing &
                bg_process = 1;
                cmd.args[cmd.num_args - 1] = '\0';
            }
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
                    printf("%d\n", pid);

                    setpgid(pid, 0);
                    // print the PID of the newly created bg process
                    struct sigaction sa;
                    sa.sa_sigaction = &handle_bg_terminate;
                    // - allow restarting if blocked for I/O
                    // - ignore signals if stopped since
                    // we only care about termination
                    sa.sa_flags = SA_NOCLDSTOP | SA_RESTART | SA_SIGINFO;
                    sigaction(SIGCHLD, &sa, NULL);
                }
                else
                {
                    // ignore terminal signals
                    // for I/O by child

                    struct sigaction sa;
                    sa.sa_handler = SIG_IGN;
                    sigaction(SIGTTIN, &sa, NULL);
                    sigaction(SIGTTOU, &sa, NULL);
                    // signal(SIGTTOU, SIG_IGN);

                    // set terminal foreground process group
                    // as the pid of the child
                    // (since we made child pgrp = pid)

                    tcsetpgrp(STDIN_FILENO, pid);

                    // wait for child
                    int wstatus;
                    waitpid(pid, &wstatus, WUNTRACED);

                    // restore terminal control
                    tcsetpgrp(STDIN_FILENO, getpgrp());
                    sa.sa_handler = SIG_DFL;
                    sigaction(SIGTTIN, &sa, NULL);
                    sigaction(SIGTTOU, &sa, NULL);
                }
            }
        }
    }
}