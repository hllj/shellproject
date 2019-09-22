#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
void parse(int *nparameter, int *concurrent, char *line, char **argv);

void execute(int *nparamter, int *concurrent, char **argv);

void runshell();

// Trim a string
char *ltrim(char *str, const char *seps);
char *rtrim(char *str, const char *seps);
char *trim(char *str, const char *seps); 