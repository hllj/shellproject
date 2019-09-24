#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>

void parse(int *nparameter, int *concurrent, char *line, char **argv, int *pos);

void execute(int *nparamter, int *concurrent, char **argv, int pos);

void saveHistoryBuff(char *historybuff, char* line);

void executeHistoryBuff(char *historybuff);

void runshell();

// Trim a string
char *ltrim(char *str, const char *seps);
char *rtrim(char *str, const char *seps);
char *trim(char *str, const char *seps); 

void print_arguments(int *nparameter, int *concurrent, char *line, char **argv, int *pos);