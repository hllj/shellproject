
#include "functions.h"

#define MAXLINE 80 /* The maximum length command */

void parse(int *nparameter, int *concurrent, char *line, char **argv)
{
    *nparameter = 0;
    while (*line != '\0') {       
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0';     
        if (strcmp(line, "&") == 0) *concurrent = 1;
        else {
            *argv++ = line;   
            (*nparameter)++;       
        }
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') 
            line++;             
     }
    *argv = '\0';             
}

void execute(int *nparameter, int *concurrent, char **argv)
{
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {    
       printf("*** ERROR: forking child process failed\n");
       exit(1);
    }
    else if (pid == 0) {   
        if (execvp(*argv, argv) < 0) {     
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
    }
    else {               
        if (*concurrent != 1) { // Not have concurrent
            while (wait(&status) != pid);       
        }                    
    }
    // printf("%d", *nparameter);
}

char *ltrim(char *str, const char *seps) {
    size_t totrim;
    if (seps == NULL) {
        seps = "\t\n\v\f\r ";
    }
    totrim = strspn(str, seps);
    if (totrim > 0) {
        size_t len = strlen(str);
        if (totrim == len) {
            str[0] = '\0';
        }
        else {
            memmove(str, str + totrim, len + 1 - totrim);
        }
    }
    return str;
}

char *rtrim(char *str, const char *seps) {
    int i;
    if (seps == NULL) {
        seps = "\t\n\v\f\r ";
    }
    i = strlen(str) - 1;
    while (i >= 0 && strchr(seps, str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }
    return str;
}

char *trim(char *str, const char *seps) {
    return ltrim(rtrim(str, seps), seps);
}

void runshell() {
    int nparameter;
    int concurrent;
    char  line[MAXLINE];            // Line of command
    char  *argv[MAXLINE / 2 + 1];   // argument list
    int run = 1;
    while (run) {
        concurrent = 0;
        printf("osh>");
        gets(line);                 // Read a line of command              
        trim(line, NULL);
        printf("\n");       
        // Parse into a list of arguments delimited by space     
        parse(&nparameter, &concurrent, line, argv);          
        if (strcmp(argv[0], "exit") == 0) //Hit exit
            exit(0);            
        execute(&nparameter, &concurrent, argv);              // Execute an command
    }
}