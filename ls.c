#include "includes.h"
#include "constants.h"
#include "types.h"
#include "utils.h"

// for directory contents
#include <dirent.h>
// for file stat
#include <sys/stat.h>
// for getpwuid
#include <pwd.h>
// for getgrgid
#include <grp.h>
// for modification time
#include <time.h>

typedef struct entry_data
{
    int valid;
    char permissions[11];
    unsigned long int hardlinks;
    char *owner;
    char *group;
    long int size;
    char mtime[100];

} entry_data;

int filter_hidden_entries(const struct dirent *entry)
{
    // removes hidden entries
    return !(entry->d_name && entry->d_name[0] == '.');
}

void get_permissions(struct stat st, char *permissions)
{
    char file_type = '?';
    if S_ISREG (st.st_mode)
        file_type = '-';
    else if S_ISDIR (st.st_mode)
        file_type = 'd';
    else if S_ISCHR (st.st_mode)
        file_type = 'c';
    else if S_ISBLK (st.st_mode)
        file_type = 'b';
    else if S_ISFIFO (st.st_mode)
        file_type = 'p';
    else if S_ISLNK (st.st_mode)
        file_type = 'l';
    // else if S_ISSOCK (st.st_mode)
    //     file_type = 's';
    // else if S_ISDOOR (st.st_mode)
    //     file_type = 'D';

    permissions[0] = file_type;
    permissions[1] = st.st_mode & S_IRUSR ? 'r' : '-';
    permissions[2] = st.st_mode & S_IWUSR ? 'w' : '-';
    permissions[3] = st.st_mode & S_IXUSR ? 'x' : '-';
    permissions[4] = st.st_mode & S_IRGRP ? 'r' : '-';
    permissions[5] = st.st_mode & S_IWGRP ? 'w' : '-';
    permissions[6] = st.st_mode & S_IXGRP ? 'x' : '-';
    permissions[7] = st.st_mode & S_IROTH ? 'r' : '-';
    permissions[8] = st.st_mode & S_IWOTH ? 'w' : '-';
    permissions[9] = st.st_mode & S_IXOTH ? 'x' : '-';
    permissions[10] = '\0';
}

void print_dir_contents(char *directory, int a_flag, int l_flag)
{
    printf("\n%s:\n", directory);

    // get directory contents
    struct dirent **entries;
    int num_entries = scandir(directory, &entries, a_flag ? NULL : filter_hidden_entries, alphasort);
    // NOTE: Order will be different from BASH, as here alphasort
    // applies strcoll, which is dependent locale.
    // By default, LC_COLLATE is dependent on your location (thx ubuntu!)
    // changing locale for desired behaviour from aplhasort is risky
    // and sorting in the manner as BASH does seems unnecessary
    // Also see: https://unix.stackexchange.com/questions/75341/specify-the-sort-order-with-lc-collate-so-lowercase-is-before-uppercase

    if ((num_entries == -1))
    {
        // scandir handles errors such as
        // ENOENT (path does not exist) and ENOTDIR(path not a dir)
        perror("Error");
        return;
    }

    if (l_flag)
    {
        // If l flag used, verbose details before filename
        // see https://www.gnu.org/software/coreutils/manual/html_node/What-information-is-listed.html#What-information-is-listed

        unsigned long long int dir_blk_sz = 0;
        char path[MAX_PATH_LEN];
        entry_data *print_data = malloc(num_entries * sizeof(entry_data));
        if (print_data == NULL && num_entries > 0)
        {
            perror("Error, make sure there is sufficient memory");
            return;
        }
        // print details of each entry
        for (int i = 0; i < num_entries; i++)
        {
            print_data[i].valid = 1;

            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, entries[i]->d_name);

            // Get information about the entry
            struct stat st;
            if (stat(path, &st) < 0)
            {
                printf("Error for %s:\n", entries[i]->d_name);
                print_data[i].valid = 0;
                perror("");
                continue;
            }

            // increase directory block size
            dir_blk_sz += st.st_blocks;

            // permissions
            get_permissions(st, print_data[i].permissions);

            // number of hardlinks
            print_data[i].hardlinks = st.st_nlink;

            // owner name
            struct passwd *pw = getpwuid(st.st_uid);
            if (pw == NULL)
            {
                printf("Error for %s:\n", entries[i]->d_name);
                print_data[i].valid = 0;
                perror("");
                continue;
            }
            // if name not available, use id
            if (pw->pw_name && strlen(pw->pw_name) > 0)
                print_data[i].owner = pw->pw_name;
            else
                sprintf(print_data[i].owner, "%u", pw->pw_uid);

            // group name
            struct group *gr = getgrgid(st.st_gid);
            if (gr == NULL)
            {
                printf("Error for %s:\n", entries[i]->d_name);
                print_data[i].valid = 0;
                perror("");
                continue;
            }
            // if name not available, use id
            if (gr->gr_name && strlen(gr->gr_name) > 0)
                print_data[i].group = gr->gr_name;
            else
                sprintf(print_data[i].group, "%u", gr->gr_gid);

            // size
            print_data[i].size = st.st_size;

            // last modification time
            int seconds_in_a_year = 365 * 24 * 60 * 60;
            char time_format[50];
            // show year by default, show time if
            // last acessed under 6 months ago ("recent")
            if (difftime(time(NULL), st.st_mtime) > (seconds_in_a_year / 2))
                strcpy(time_format, "%b %e %Y");
            else
                strcpy(time_format, "%b %e %R");
            strftime(print_data[i].mtime, sizeof(print_data[i].mtime), time_format, localtime(&st.st_mtime));
        }

        // Print the total number of blocks
        // Note: this may not match with BASH, since the BLOCK_SIZE
        // is 1024 by default on it while in stat it is 512
        printf("total %llu\n", dir_blk_sz);

        for (int i = 0; i < num_entries; i++)
        {
            if (print_data[i].valid)
            {
                printf("%s %*lu %*s %*s %*ld %s %s\n",
                       print_data[i].permissions,
                       3, print_data[i].hardlinks,
                       10, print_data[i].owner,
                       10, print_data[i].group,
                       10, print_data[i].size,
                       print_data[i].mtime,
                       entries[i]->d_name);
            }
            else
            {
                printf("? %s:\n", entries[i]->d_name);
            }
        }

        free(print_data);
    }
    else
    {
        for (int i = 0; i < num_entries; i++)
        {
            printf("%s\n", entries[i]->d_name);
        }
    }

    free(entries);
}

void ls(command cmd)
{
    // for getopt()
    extern int optind;
    optind = 1;

    // flags
    int flag;
    int a_flag = 0;
    int l_flag = 0;

    // find if any flags used
    while ((flag = getopt(cmd.num_args, cmd.args, "la")) != -1)
    {
        switch (flag)
        {
        case 'l':
            l_flag += 1;
            break;
        case 'a':
            a_flag += 1;
            break;
        default:
            // unknown flag used, getopt handles this error
            return;
        }
    }
    // iterate arguments for directories
    int arg_flag = 1;
    int i = 1;
    char dir_path[MAX_PATH_LEN];
    for (i = 1; i < cmd.num_args; i++)
    {
        if (cmd.args[i][0] != '-')
        {
            strcpy(dir_path, cmd.args[i]);
            replace_tilde_with_home(dir_path);
            print_dir_contents(dir_path, a_flag, l_flag);
            arg_flag = 0;
        }
    }

    // no directories given, so current dir
    if (arg_flag)
    {
        print_dir_contents("./", a_flag, l_flag);
    }
}