//
//  mysh.c
//  
//
//  Created by Chen Gong on 4/4/21.
//

#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <inttypes.h>

typedef struct {
    char* arg;
    char* inFile;
    char* outFile;
} Cmd;

typedef struct {
    Cmd** cmdArray;
    int ifBackground;
} CmdSet;

char* prompt = "mysh:";

extern int errno ;

void set_prompt (int argc, char* argv[]) {
    //first
    char* program_name = argv[0];
    if (argc > 2)
        fprintf(stderr, "Error: Usage: %s [prompt]\n", program_name);
    
    if (argc == 2) {
        if (!strcmp(argv[1], "-")) {
            prompt = "";
        } else {
            prompt = strcat(argv[1], " ");
        }
    }
}

// return cmdSet
char** get_cmd() {
    //Check fgets() and wait() for premature returns due to system interruption: if fgets() or wait() fails and errno == EINTR, try the call again!
    char line[1024];
    //EINTR_flag
    //while EINTR_flag not true
    //do fgets
    int EINTR_flag = 0;
    do {
        EINTR_flag = 0;
        if(fgets(line, 1024, stdin) == NULL) {
//            fprintf(stderr, "Errno: %s", strerror( errno ) );
            perror("fgets: ");
            if (errno == EINTR) {
                EINTR_flag = 1;
            } else {
                break;
            }
        }
    } while (EINTR_flag);
    if (line[0] == '\n')
        return NULL;
    char** token = get_tokens(line);
    return token;
}



void display_prompt() {
    printf("%s", prompt);
}


int main (int argc, char* argv[]) {
    
    //check if prompt offered, no prompt, will print mysh:
    set_prompt(argc, argv);
    

    
    //char** token = get_tokens(argv);
    
    //Check fgets() and wait() for premature returns due to system interruption: if fgets() or wait() fails and errno == EINTR, try the call again!
    
    //char** token;
    //int pid;
    
    
    while(1) {
        display_prompt();
        int pid;
        char**token = get_cmd();
        if (token == NULL) {// if no command entered, print another prompt
            continue;
        }
        printf("%s\n", token[0]);
        if( token[0][0]== EOF || strcmp(token[0], "exit") == 0){       //printf("Goodbye!\n");
            exit(0);
        }

 
        pid = fork();
        sleep(3);
        if (pid == 0) {
            //make sure execvp did not unexpected fail
            //printf("child process\n");
            int EINTR_flag = 0;
            do {
                EINTR_flag = 0;
                if(execvp(token[0], token) == -1) {
    //                fprintf(stderr, "errno: %s\n", strerror( errno ) );
                    perror(token[0]);
                    if (errno == EINTR) {
                        EINTR_flag = 1;
                    }
                }
                exit(-1);
                
            } while (EINTR_flag);
            //execvp(token[0], token);
            
        } else {
            int wpid;
            int status;
            wpid = wait(&status);
            if (wpid == -1) {
                printf("\nParent (%d): wait(): %s\n", getpid(), strerror(errno));
            } else {
                //printf("parent process, waited wpid: %d with status:%d\n", wpid, status);
            }
        }
        free_tokens(token);
    }
}
