#ifndef __UTILS_H
#define __UTILS_H

void read_input(char **input);

void trim(char *str);

void tokenise(char *str, char *delim, char **tokens, int *num_tokens);

#endif