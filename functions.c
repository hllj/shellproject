
#include "functions.h"

#define MAXLINE 80 /* The maximum length command */

void parse(char *line, char **argv)
{
     while (*line != '\0') {       
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     
          *argv++ = line;          
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             
     }
     *argv = '\0';                 
}

void  execute(char **argv)
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
        while (wait(&status) != pid);       
     }
}

void runshell() {
    char  line[MAXLINE];            // Line of command
    char  *argv[MAXLINE / 2 + 1];   // argument list
    int run = 1;
    while (run) {
        printf("osh>");
        gets(line);                 // Read a line of command              
        printf("\n");       
        parse(line, argv);          // Parse into a list of arguments delimited by space     
        if (strcmp(argv[0], "exit") == 0) //Hit exit
            exit(0);            
        execute(argv);              // Execute an command
    }
}