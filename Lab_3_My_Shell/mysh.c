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
#include <fcntl.h>

typedef struct {
    char* inFile;
    char* outFile;
    int pipe_to;
    int read_from_pipe;
    int append;
    int argnum;
    char* arg[256];
} Cmd;

typedef struct {
    Cmd* cmdArray[256];
    int cmdNum;
    int ifBackground;
} CmdSet;

char* prompt = "mysh:";

extern int errno ;

void set_prompt (int argc, char* argv[]) {
    //first
    char* program_name = argv[0];
    if (argc > 2) {
        fprintf(stderr, "Error: Usage: %s [prompt]\n", program_name);
        exit(-1);
    }
            
    if (argc == 2) {
        if (!strcmp(argv[1], "-")) {
            prompt = "";
        } else {
            prompt = strcat(argv[1], " ");
        }
    }
}

int set_cmds(CmdSet* cmdSet, char** tokens);

// return cmdSet
void helper_printcmds(CmdSet* cmdSet) {
    //printf("enterred print cmd\n");
    Cmd* c;
    //printf("first: %s\n", cmdSet -> cmdArray[0] -> arg[0]);
    c = cmdSet -> cmdArray[0];
    //printf("cmd : %s\n", c -> arg[0]);
    for (int j = 0; j <= cmdSet -> cmdNum; j++) {
        c = cmdSet -> cmdArray[j];
//        for (int i = 0; i < c -> argnum; i++) {
//            printf("arg[%d]: %s\n", i, c -> arg[i]);
//        }
        fprintf(stderr, "in helper inFile: %s\n", c -> inFile);
    }

   // printf("exited print cmd\n");
}

//void init_cmdSet
//return -1 on fail
int get_cmd(CmdSet* cmds) {
    //Check fgets() and wait() for premature returns due to system interruption: if fgets() or wait() fails and errno == EINTR, try the call again!
//    CmdSet* cmds = malloc(sizeof(CmdSet));
    
    
    char line[1024];
    //EINTR_flag
    //while EINTR_flag not true
    //do fgets
    int EINTR_flag = 0;
    do {
        EINTR_flag = 0;
        if(fgets(line, 1024, stdin) == NULL) {
//            fprintf(stderr, "Errno: %s", strerror( errno ) );
            if (errno == EINTR) {
                EINTR_flag = 1;
            } else {
                if (feof(stdin)) {
                    exit(0);
                }
                break;
            }
        }
    } while (EINTR_flag);
    //printf("fgets successed\n");
    if (line[0] == '\n')
        return -1;
    char** tokens = get_tokens(line);
    //set_cmd(tokens);
    if (set_cmds(cmds, tokens) == -1) {
        return -1;
    }
    //printf("set cmd succeed\n");
    //helper_printcmds(cmds);
   // return tokens;
    return 0;
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
    else if (strcmp(token, "&") == 0)
        return 1;
    return 0;
}

int is_pipe(char* token) {
    if (strcmp(token, "|") == 0)
        return 1;
    return 0;
}

int check_pipe(CmdSet* cmdSet, Cmd* cmd) {
    if (cmd -> outFile != NULL) {
        fprintf (stderr, "Error: Ambiguous output redirection.\n");
        return -1;
    }
    cmd -> pipe_to = 1;
    
    return 0;
}

//return -1 on fail
int set_operator(CmdSet* cmdSet, Cmd* cmd, char** tokens, int* p) {
    //set_operator might use tokens[*p] as operator and increase *p;
    char* token = tokens[*p];
    if (strcmp(token, "<") == 0) {

        ++(*p);
//        printf("token p+1 : %s\n", tokens[*p]);
        if ( tokens[*p] == NULL || check_operator(tokens[*p])) {
            fprintf (stderr, "Error: Missing filename for input redirection.\n");
            return -1;
        }
        //if input FIle already set, or stdin from pipe.
        if (cmd -> inFile != NULL || cmd -> read_from_pipe) {
            fprintf (stderr, "Error: Ambiguous input redirection.\n");
            return -1;
        }
        //fprintf(stderr, "set inFile\n");
        cmd -> inFile = tokens[*p];
    }
    else if (strcmp(token, ">") == 0) {
        ++(*p);
        if ( tokens[*p] == NULL || check_operator(tokens[*p])) {
            fprintf (stderr, "Error: Missing filename for output redirection.\n");
            return -1;
        }
        if (cmd -> outFile != NULL) {
            fprintf (stderr, "Error: Ambiguous output redirection.\n");
            return -1;
        }
        cmd -> append = 0;
        cmd -> outFile = tokens[*p];
        //printf("cmd -> outFile: %s\n", cmd -> outFile);
    }
    else if (strcmp(token, ">>") == 0) {
        //if append
        ++(*p);
        if ( tokens[*p] == NULL || check_operator(tokens[*p])) {
            fprintf (stderr, "Error: Missing filename for output redirection.\n");
            return -1;
        }
        if (cmd -> outFile != NULL) {
            fprintf (stderr, "Error: Ambiguous output redirection.\n");
            return -1;
        }
        cmd -> append = 1;
        cmd -> outFile = tokens[*p];
    }
    else if (strcmp(token, "|") == 0) {
//        if (cmd -> outFile != NULL) {
//            fprintf (stderr, "Error: Ambiguous output redirection.\n");
//            return -1;
//        }
    }
    else if (strcmp(token, "&") == 0) {
        if (tokens[*p + 1] != NULL) {
            fprintf (stderr, "Error: \"&\" must be last token on command line\n");
            return -1;

        } else {
            cmdSet -> ifBackground = 1;
        }
    }
    return 0;
    
}



//return -1 on failure
int set_cmds(CmdSet* cmdSet, char** tokens) {
    //printf("set_cmds enterred\n");
    cmdSet -> ifBackground = 0;
    //printf("1\n");
    int newCmd = 0;
    int i = 0;
    int cmdCount = 0;
    int argCount = 0;
    Cmd* c = cmdSet -> cmdArray[0];
    c -> pipe_to = 0;
//    printf("address c: %d\n", c);
//    printf("address cmdArray[0]: %d\n", cmdSet -> cmdArray[0]);
    while (tokens[i] != NULL) {
        //if it's a operator, set the input/output file and update operator.  Pipe not inplemented.
        //if a pipe, next part would be new command.
        if (check_operator(tokens[i])) {
            //an operator
            //fprintf(stderr, "is a operator\n");
            if (!is_pipe(tokens[i])) {
                //fprintf(stderr, "not a pipe\n");
                if (set_operator(cmdSet, c, tokens, &i) == -1) {
                    return -1;
                }

            } else {
                //check pipe;
                if (check_pipe(cmdSet, c) == -1)
                    return -1;
                //fprintf(stderr, "pipe_to %d\n", c -> pipe_to);
                newCmd = 1;
                //set c to next cmd pointer
//                c = cmdSet -> cmdArray[++cmdCount];
//                c -> read_from_pipe = 1;
                //start counting the argument for cmd
                //argCount = 0;
                //fprintf(stderr, "c -> inFile: %s\n", c -> inFile);
            }
            //printf("after set_operator\n");
            i++;
            continue;
        }
        
        //newCmd
        if (newCmd) {
            //set c to next cmd pointer
            //fprintf(stderr, "==== new command: %s\n", tokens[i]);
            c = cmdSet -> cmdArray[++cmdCount];
            c -> read_from_pipe = 1;
            c -> pipe_to = 0;
            //start counting the argument for cmd
            argCount = 0;
            //fprintf(stderr, "2 inFile: %s\n", cmdSet -> cmdArray[0] -> inFile);
        }
        //printf("3\n");
        //printf("token %s\n", tokens[i]);
        //fprintf(stderr, "2.25 inFile: %s\n", cmdSet -> cmdArray[0] -> inFile);
        //fprintf(stderr, "1c -> arg: %s\n", c -> arg[argCount]);
        
        c -> arg[argCount++] = tokens[i];
        //fprintf(stderr, "2c -> arg: %s\n", c -> arg[argCount - 1]);
        //c -> arg[0] = tokens[i];
        //fprintf(stderr, "2.5 inFile: %s\n", cmdSet -> cmdArray[0] -> inFile);
        c -> argnum = argCount;
        //printf("5\n");
        i++;
        //fprintf(stderr, "3 inFile: %s\n", cmdSet -> cmdArray[0] -> inFile);
        
    }
    //printf("command num: %d\n", cmdCount);
    cmdSet -> cmdNum = cmdCount;
    Cmd* tmp;
    for (int i = 0; i <= cmdCount; i++ ) {
        tmp = cmdSet -> cmdArray[i];
        if (tmp -> arg[0] == NULL) {
            fprintf(stderr, "Error: invalid null command\n");
            return -1;
        }
            
    }
    //fprintf(stderr, "after setcmd inFile: %s\n", cmdSet -> cmdArray[0] -> inFile);
    return 0;

}

void display_prompt() {
    printf("%s", prompt);
}


//if pid in pid list, return 1, else return 0;
int check_pid (int pid_list[], int pid, int cmdNum) {
//    for(int i = 0; i <= cmdNum; i++) {
//        printf("pid[%d]:%d\n",i, pid_list[i]);
//    }
    for (int i = 0; i <= cmdNum; i++) {
        if (pid_list[i] == pid)
            return 1;
    }
    return 0;
}

void init_cmdSet (CmdSet* cmds) {
    for (int i = 0; i < 256; i++) {
        cmds -> cmdArray[i] = calloc(256, sizeof(char*));
    }
}

void free_cmdSet(CmdSet* cmds) {
    //
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
    set_prompt(argc, argv);
    
    
    //char** token = get_tokens(argv);
    //Check fgets() and wait() for premature returns due to system interruption: if fgets() or wait() fails and errno == EINTR, try the call again!
    
    //char** token;
    //int pid;
    
    while(1) {
        display_prompt();
        int pid;
        int pid_list[256];
//        CmdSet* cmds;
//        cmds = get_cmd();
        CmdSet cmds;
        init_cmdSet(&cmds);
        int pipefd[2] = {-1, -1};
        int prevpipefd[2] = {-1, -1};
//        char**token = get_cmd();
        if (get_cmd(&cmds) == -1) {// if no command entered, print another prompt
            continue;
        }
        
        int n = 0;
        Cmd* c;
        //printf("cmdNum: %d\n", cmds.cmdNum);
//        printpipe(pipefd);
//        printprevpipe(prevpipefd);
        
        
        while (n <= (cmds.cmdNum)) {
            c = cmds.cmdArray[n];
            //fprintf(stderr, "inFile: %s\n", c -> inFile);
            if( strcmp(c -> arg[0], "exit") == 0){       //printf("Goodbye!\n");
                exit(0);
            }
            
            if (c -> pipe_to)
                pipe(pipefd);
            
            //printpipe(pipefd);
            //printprevpipe(prevpipefd);
            pid = fork();
            //sleep(1);
            
        
            if (pid == 0) {
                //child process
                //make sure execvp did not unexpected fail
                //printf("child process cmd[%d]\n", n);
                if (n == 0) {
                    //the first cmd probably input file redirection.
                    if (c -> inFile != NULL) {
                        int fd = open(c -> inFile, O_RDONLY);
                        if (fd < 0) {
                            fprintf(stderr, "Error1111: open(\"%s\"): %s\n", c -> inFile, strerror(errno) );
                            fprintf(stderr, "Error: open(\"%s\"): %s\n", c -> inFile, strerror(errno) );
                            exit(-1);
                        }
                        dup2(fd, 0);
                        close (fd);
                        //printf("input set\n");
                    }
                    if (c -> outFile != NULL) {
                        int fd;
                        if (c -> append) {
                            //printf(">> append\n");
                            fd = open (c -> outFile, O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP |
                                       S_IROTH);
                        } else {
                            //how to check if a file is exist;
                            //printf("> creat\n");
                            fd = open (c -> outFile, O_CREAT | O_WRONLY  | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP |
                                       S_IROTH);
                        }
                        if (fd < 0) {
                            fprintf(stderr, "Error: open(\"%s\"): %s\n", c -> outFile, strerror(errno) );
                            exit(-1);
                        }
                        dup2(fd, 1);
                        close (fd);
                        //printf("output set\n");
                    }
                    //printf("1\n");
                    if (c -> pipe_to) {
                        //fprintf(stderr, "pipe to\n");
                        dup2(pipefd[1], 1);
                        //fprintf(stderr, "redirect 1 to %d\n", pipefd[1]);
                        close(pipefd[1]);
                        close(pipefd[0]);
                    }
                    
                    //printf("2\n");
                } else if (n == cmds.cmdNum) {
                    // the last command
                    //printf("the last command\n");
                    //printprevpipe(prevpipefd);
                    dup2(prevpipefd[0], 0);
                    //printf("redirect 0 to %d\n", prevpipefd[0]);
                    close(prevpipefd[0]);
                    close (prevpipefd[1]);
                    if (c -> outFile != NULL) {
                        //printf("outputFile: %s\n", c -> outFile);
                        int fd;
                        if (c -> append) {
                            //printf(">> append\n");
                            fd = open (c -> outFile, O_APPEND | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP |
                                       S_IROTH);
                        } else {
                            //how to check if a file is exist;
                            //printf("> creat\n");
                            fd = open (c -> outFile, O_CREAT | O_WRONLY  | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP |
                                       S_IROTH);
                        }
                        if (fd < 0) {
                            //fprintf(stderr, "Error22222: open(\"%s\"): %s\n", c -> outFile, strerror(errno) );
                            fprintf(stderr, "Error: open(\"%s\"): %s\n", c -> outFile, strerror(errno) );
                            exit(-1);
                        }
                        dup2(fd, 1);
                        close (fd);
                    }
                    
                } else {
                    //commands between first and last one
                    dup2(prevpipefd[0], 0);
                    close(prevpipefd[0]);
                    close(prevpipefd[1]);
                    dup2(pipefd[1], 1);
                    close(pipefd[1]);
                    close (pipefd[0]);
                }
                
                int EINTR_flag = 0;
                do {
                    EINTR_flag = 0;
                    if(execvp(c -> arg[0], c -> arg) == -1) {
                        perror(c -> arg[0]);
                        if (errno == EINTR) {
                            EINTR_flag = 1;
                        }
                    }
                    exit(-1);
                    
                } while (EINTR_flag);
            }
            pid_list[n] = pid;
            //printf("pid_list[%d]: %d\n", n, pid);
            prevpipefd[0] = pipefd[0];
            prevpipefd[1] = pipefd[1];
            close(prevpipefd[1]);
            //printpipe(pipefd);
            //printprevpipe(prevpipefd);
            
            n++;
        }
//==================parent part ============
        //stores the previous pipefd

        //background or forground
        if (cmds.ifBackground) {
            //background, not waiting
            printf("background running\n");
            
        } else {
            //foreground, waiting
            int wpid_count = 0;
            while (wpid_count <= cmds.cmdNum) {
                //sleep(1);
                int wpid;
                int status;
                //printf("===parent waiting\n");
                wpid = wait(&status);
                if (check_pid(pid_list, wpid, cmds.cmdNum)) {
                    //printf("wpid: %d\n", wpid);
                    wpid_count++;
                } else {
                    printf("zombie process: %d\n", wpid);
                }
                if (wpid == -1) {
                    printf("\nParent (%d): wait(): %s\n", getpid(), strerror(errno));
                } else {
                    //printf("parent process, waited wpid: %d with status:%d\n", wpid, status);
                }
            }
            
            
        }

    }
}
