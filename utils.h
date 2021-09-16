#ifndef __UTILS_H
#define __UTILS_H

// DEPRECATED
// void read_input(char **input);

void trim(char *str);

void tokenise(char *str, char *delim, char **tokens, int *num_tokens);

void replace_tilde_with_home(char *path, char *shell_home_path);

#endif