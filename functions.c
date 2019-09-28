
#include "functions.h"

#define MAXLINE 80 /* The maximum length command */
#define BUFFSIZE 1000

void parse(int *nparameter, int *concurrent, char *line, char **argv, int *pos)
{
    *nparameter = 0;

    while (*line != '\0') {       
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0';     
        if (strcmp(line, "&") == 0) *concurrent = 1;
        else {
            if (*line == '|' || *line == '>' || *line == '<') {
                *pos = *nparameter;
            } 

            if (*line == '"'){
                
                int doubleQuotes = 0;
                for (int i = 1; *(line + i) != '\0'; i++){
                    if (*(line + i) == '"'){
                        *(line + i) = '\0';
                        *line++ = '\0';
                        
                        *argv++ = line;
                        (*nparameter)++;
                        
                        int j = 0;
                        while (j < i) {
                            *line++;
                            j++;
                        }
                        doubleQuotes = 1;
                        break;
                    } 
                }

                if (doubleQuotes == 0){
                    *argv++ = line;   
                    (*nparameter)++; 
                }

            } else{
                *argv++ = line;  
                (*nparameter)++; 
            }
        }
        
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n' && *line != '"') 
            *line++;             
     }

    *argv = '\0';
}

void execute(int *nparameter, int *concurrent, char **argv, int pos)
{
    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {    
       printf("*** ERROR: forking child process failed\n");
       exit(1);
    }
    else 
    if (pid == 0) {   
        if (pos == -1) {
            if (execvp(*argv, argv) < 0) {     
                printf("*** ERROR: exec failed\n");
                exit(1);
            }
        }
        else {
            int i;
            char *new_argv[MAXLINE/2+2];
            for (i = 0; i<pos ;i++){
                new_argv[i] = malloc(strlen(argv[i]) + 1);
                strcpy(new_argv[i],argv[i]);
            }
            new_argv[pos] = '\0';

            if (strcmp(argv[pos],">") == 0) {
                int fd = open(argv[pos+1] , O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                dup2(fd, 1);
                close(fd);

                if (execvp(*new_argv, new_argv) < 0) {  
        
                    printf("*** ERROR: exec failed\n");
                    for (i = 0; i<pos ;i++){
                        free(new_argv[i]);
                    }
                    exit(1);
                }
                
            }else if (strcmp(argv[pos],"<") == 0) {
                
                int fd = open(argv[pos+1] , O_RDONLY);
                dup2(fd,0);
                close(fd);

                if (execvp(*new_argv, new_argv) < 0) {     
                    printf("*** ERROR: exec failed\n");
                    int i; 
                    for ( i = 0; i<pos ;i++){
                        free(new_argv[i]);
                    }
                    exit(1);
                }
            }else if (strcmp(argv[pos],"|") == 0) {
                
                int pfd[2];
                pipe(pfd);
                int cpid;
                cpid = fork();

                if (cpid == 0){
                    close(pfd[0]);
                    dup2(pfd[1],1);
 
                    if (execvp(*new_argv, new_argv) < 0) {     
                        printf("*** ERROR: exec failed\n");
                        for (i = 0; i<pos ;i++){
                            free(new_argv[i]);
                        }
                        exit(1);
                    }   
                    
                }
                else if (cpid > 0) {   
                    char *new_argv[MAXLINE/2+2];
                    int stt;
                    wait(&stt);
                    close(pfd[1]);
                    dup2(pfd[0],0);
                    int i;

                    for ( i = pos +1; i<*nparameter ;i++){
                        new_argv[i-pos-1] = malloc(strlen(argv[i]) + 1);
                        strcpy(new_argv[i-pos-1],argv[i]);
                    }

                    new_argv[*nparameter- pos-1] = '\0';

                    if (execvp(*new_argv, new_argv) < 0) {     

                        printf("*** ERROR: exec failed1\n");
                        for (i = 0; i<*nparameter- pos-1;i++){
                            free(new_argv[i]);
                        }
                        exit(1);
                    }   
                }
                
            }
            for (i = 0; i<pos ;i++)
                free(new_argv[i]);
        }
    }
    else {         
        // Not have concurrent      
        if (*concurrent != 1) { 
            while (wait(&status) != pid);       
        }                    
    }
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

void saveHistoryBuff(char *historybuff, char *line) {
    strcpy(historybuff, line);
}

int executeCD(char* path){
    return chdir(path);
}

void runshell() {
    int nparameter;
    int concurrent;
    int pos;
    // Line of command
    char  line[MAXLINE];   
    // Save a line         
    char  buffline[MAXLINE];
    // argument list        
    char  *argv[MAXLINE / 2 + 1];   
    char historybuff[MAXLINE];
    strcpy(historybuff, "NULL");

    int run = 1;

    while (run) {
        concurrent = 0;
        pos = -1;
        printf("osh>");
        // Read a line of command 
        fgets(line, MAXLINE, stdin);                              
        trim(line, NULL);
        
        // Save another line command before parsed into argv
        strcpy(buffline, line);     
         
        // Parse into a list of arguments delimited by space     
        parse(&nparameter, &concurrent, line, argv, &pos);
        
        // Execute history command
        if (strcmp(argv[0], "!!") == 0) {
            // There is recent command in the history
            if (strcmp(historybuff, "NULL") != 0){
                strcpy(line, historybuff);
                strcpy(buffline, line);  

                printf("osh> %s\n", line);
                parse(&nparameter, &concurrent, line, argv, &pos);
            } // There is no recent command in the history
            else{
                printf("No commands in history.\n");
                continue;
            }
        }    
        
        // Execute cd command
        if (strcmp(argv[0], "cd") == 0){
            char dir[MAXLINE];
            strcpy(dir, argv[1]);
            
            if (strcmp(dir, "~") == 0) {
                strcpy(dir, getenv("HOME"));
            }
            
            if (executeCD(dir) < 0)
                printf("cd: %s: No such file or directory\n", argv[1]);
        }
        else
        // Hit exit
        if (strcmp(argv[0], "exit") == 0) 
            exit(0);   
        else {
            // Execute another command   
            execute(&nparameter, &concurrent, argv, pos);  
            printf("\n");
        }
        
        saveHistoryBuff(historybuff, buffline);
    }
}

