//
//  test.c
//  
//
//  Created by 龚晨 on 4/9/21.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <inttypes.h>

char* prompt = "mysh: ";

int errno;

char ** get_tokens( const char * line ) {
    char **tokens=NULL;
    char * line_copy;
    const char * delim = " \t\n";
    char * cur_token;
    int num_tokens=0;
    
    tokens = (char**) malloc( sizeof(char*) );
    tokens[0] = NULL;
    
    if( line == NULL )
        return tokens;
    
    line_copy = strdup( line );
    cur_token = strtok( line_copy, delim );
    
    if( cur_token == NULL )
        return tokens;
    
    do {
        num_tokens++;
        tokens = ( char ** ) realloc( tokens, (num_tokens+1) * sizeof( char * ) );
        tokens[ num_tokens - 1 ] = strdup(cur_token);
        tokens[ num_tokens ] = NULL;
    } while( (cur_token = strtok( NULL, delim )) );
    free(line_copy);
    
    return tokens;
}

void set_prompt (int argc, char* argv[]) {
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
void set_cmd(char** tokens);

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
    char** tokens = get_tokens(line);
    set_cmd(tokens);
    return tokens;
}


int check_operator(char* token) {
    if (strcmp(token, "<") == 0)
        return 1;
    else if (strcmp(token, ">") == 0)
        return 1;
    else if (strcmp(token, ">>") == 0)
        return 1;
    else if (strcmp(token, "|") == 0)
        return 1;
    return 0;
}

void set_cmd(char** tokens) {
    
    for (int i = 0; tokens[i] != NULL; i++) {
        char c = check_operator(tokens[i]);
        if (c == 0) {
            printf("tokens[%d]: is not operator\n", i);
        } else {
            printf("tokens[%d]: is operator '%c'\n", i, tokens[i][0]);
        }
    }
    
    
}
void printpipe(int a[]) {
    fprintf(stderr, "pipe 0: %d\n", a[0]);
    fprintf(stderr, "pipe 1: %d\n", a[1]);
}
void printprevpipe(int a[]) {
    fprintf(stderr, "prevpipe 0: %d\n", a[0]);
    fprintf(stderr, "prevpipe 1: %d\n", a[1]);
}

int main (int argc, char* argv[]) {
    
    //check if prompt offered, no prompt, will print mysh:
    //set_prompt(argv[]);
    
    int pipefd[2] = {-1, -1};
    int prevpipefd[2] = {-1, -1};
    
    pipe(pipefd);
    printpipe(pipefd);
    printprevpipe(prevpipefd);

    
    for (int i = 0; i < 2; i++) {
        int pid = fork();
        if (pid ==0) {
            if (i == 0) {
                dup2(pipefd[1], 1);
                close(pipefd[1]);
                exit(0);
            } else {
                printpipe(pipefd);
                printprevpipe(prevpipefd);
                dup2(prevpipefd[0], 0);
                close(prevpipefd[0]);
                exit(0);
            }
        }

        prevpipefd[0] = pipefd[0];
        prevpipefd[1] = pipefd[1];
        printpipe(pipefd);
        printprevpipe(prevpipefd);
        
        
    }
    
}



//can't find the difference between the two do-while
//char** get_cmd1() {
//    //Check fgets() and wait() for premature returns due to system interruption: if fgets() or wait() fails and errno == EINTR, try the call again!
//    char line[1024];
//    //EINTR_flag
//    //while EINTR_flag not true
//    //do fgets
//    int EINTR_flag = 0;
//    do {
//        EINTR_flag = 0;
//        if(fgets(line, 1024, stdin) == NULL) {
////            fprintf(stderr, "Errno: %s", strerror( errno ) );
//            perror("fgets: ");
//            if (errno == EINTR) {
//                EINTR_flag = 1;
//            } else {
//                break;
//            }
//        }
//    } while (EINTR_flag);
////    do {
////        EINTR_flag = 0;
////        if(fgets(line, 1024, stdin) == NULL) {
//////            fprintf(stderr, "Errno: %s", strerror( errno ) );
////            perror("fgets: ");
////            if (errno == EINTR) {
////                EINTR_flag = 1;
////            } else {
////                break;
////            }
////        }
////    } while (EINTR_flag);
//
//    if (line[0] == '\n')
//        return NULL;
//    char** tokens = get_tokens(line);
//    CmdSet cmds;
//    if (set_cmds(&cmds, tokens) == -1) {
//        return NULL;
//    }
//    helper_printcmds(&cmds);
//    return tokens;
//}
