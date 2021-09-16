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

typedef struct entry_data
{
    int valid;
    char permissions[11];
    unsigned long int hardlinks;
    char *owner;
    char *group;
    long int size;

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
    printf("%s:\n", directory);

    // get directory contents
    struct dirent **entries;
    int num_entries = scandir(directory, &entries, a_flag ? NULL : filter_hidden_entries, alphasort);
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

            // block size
            dir_blk_sz += st.st_blocks;
            // permissions
            get_permissions(st, print_data[i].permissions);
            // number of hardlinks
            print_data[i].hardlinks = st.st_nlink;
            // owner name
            print_data[i].owner = getpwuid(st.st_uid)->pw_name;
            // group name
            print_data[i].group = getgrgid(st.st_gid)->gr_name;
            // size
            print_data[i].size = st.st_size;
        }

        // Print the number of blocks
        // Note: this may not match with BASH, since the BLOCK_SIZE
        // is 1024 by default on it while in stat it is 512
        printf("total %llu\n", dir_blk_sz);

        for (int i = 0; i < num_entries; i++)
        {
            printf("%s %lu %*s %*s %*ld %s\n",
                   print_data[i].permissions,
                   print_data[i].hardlinks,
                   5, print_data[i].owner,
                   5, print_data[i].group,
                   5, print_data[i].size,
                   entries[i]->d_name);
        }

        free(print_data);
    }
    else
    {
        for (int i = 0; i < num_entries; i++)
        {
            printf("%s ", entries[i]->d_name);
        }
        printf("\n");
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
    int i = 1;
    for (i = 1; i < cmd.num_args; i++)
    {
        if (cmd.args[i][0] != '-')
        {
            replace_tilde_with_home(cmd.args[i], cmd.internal_args);
            print_dir_contents(cmd.args[i], a_flag, l_flag);
        }
    }

    // no directories given, so current dir
    if (i == cmd.num_args)
    {
        print_dir_contents(".", a_flag, l_flag);
    }
}