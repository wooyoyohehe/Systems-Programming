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
    char* arg[256];
    char* inFile;
    char* outFile;
    int append;
    int argnum;
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

int set_cmds(CmdSet* cmdSet, char** tokens);

// return cmdSet
void helper_printcmds(CmdSet* cmdSet) {
    printf("enterred print cmd\n");
    Cmd* c;
    //printf("first: %s\n", cmdSet -> cmdArray[0] -> arg[0]);
    c = cmdSet -> cmdArray[0];
    printf("cmd : %s\n", c -> arg[0]);
    for (int j = 0; j <= cmdSet -> cmdNum; j++) {
        c = cmdSet -> cmdArray[j];
        for (int i = 0; i < c -> argnum; i++) {
            printf("arg[%d]: %s\n", i, c -> arg[i]);
        }
    }

    printf("exited print cmd\n");
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
            perror("fgets: ");
            if (errno == EINTR) {
                EINTR_flag = 1;
            } else {
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
   // helper_printcmds(cmds);
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
        if (cmd -> inFile != NULL) {
            fprintf (stderr, "Error: Ambiguous input redirection.\n");
            return -1;
        }
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

//int set_cmds(CmdSet* cmdSet, char** tokens) {
//    cmdSet -> ifBackground = 0;
//    int newCmd = 0;
//    for (int i = 0; tokens[i] != NULL; i++) {
//        Cmd c;
//        if (check_operator(tokens[i])) {
//            //an operator
//            if (set_operator(cmdSet, &c, tokens, &i) == -1) {
//                return -1;
//            }
//
//        }
//        printf("cmd.file: %s\n", c.inFile);
//    }
//    printf("ifBackground: %d\n", cmdSet -> ifBackground);
//
//    return 0;
//
//}


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
//    printf("address c: %d\n", c);
//    printf("address cmdArray[0]: %d\n", cmdSet -> cmdArray[0]);
    while (tokens[i] != NULL) {
        //if it's a operator, set the input/output file and update operator.  Pipe not inplemented.
        //if a pipe, next part would be new command.
        if (check_operator(tokens[i])) {
            //an operator
            //printf("is a operator\n");
            if (!is_pipe(tokens[i])) {
                //printf("not a pipe\n");
                if (set_operator(cmdSet, c, tokens, &i) == -1) {
                    return -1;
                }
            } else {
                newCmd = 1;
            }
            //printf("after set_operator\n");
            i++;
            continue;
        }
        
        //newCmd
        if (newCmd) {
            //set c to next cmd pointer
            c = cmdSet -> cmdArray[++cmdCount];
            //c = malloc(sizeof(Cmd));
            argCount = 0;
            /*set the cmd name not have to be the first one.? check example */
            //可以加一个command set？ flag在前面，没set就set， else 直接加到argument。
            //或着根本不用check if set， 全都加到argument就行了。 能到这里的都不是operator。
            
            //c -> arg[0] = tokens[i];
        }
        //printf("3\n");
        //printf("token %s\n", tokens[i]);
        c -> arg[argCount++] = tokens[i];
        c -> argnum = argCount;
        //printf("5\n");
        i++;
        
    }
    //printf("command num: %d\n", cmdCount);
    cmdSet -> cmdNum = cmdCount;
    return 0;

}

void display_prompt() {
    printf("%s", prompt);
}


//int main2 (int argc, char* argv[]) {
//
//    //check if prompt offered, no prompt, will print mysh:
//    set_prompt(argc, argv);
//
//
//    //char** token = get_tokens(argv);
//    //Check fgets() and wait() for premature returns due to system interruption: if fgets() or wait() fails and errno == EINTR, try the call again!
//
//    //char** token;
//    //int pid;
//
//
//    while(1) {
//        display_prompt();
//        int pid;
//        char**token = get_cmd();
//        if (token == NULL) {// if no command entered, print another prompt
//            continue;
//        }
//        //printf("%s\n", token[0]);
//        if( token[0][0]== EOF || strcmp(token[0], "exit") == 0){       //printf("Goodbye!\n");
//            exit(0);
//        }
//
//        pid = fork();
//        sleep(3);
//        if (pid == 0) {
//            //make sure execvp did not unexpected fail
//            //printf("child process\n");
//            int EINTR_flag = 0;
//            do {
//                EINTR_flag = 0;
//                if(execvp(token[0], token) == -1) {
//    //                fprintf(stderr, "errno: %s\n", strerror( errno ) );
//                    perror(token[0]);
//                    if (errno == EINTR) {
//                        EINTR_flag = 1;
//                    }
//                }
//                exit(-1);
//
//            } while (EINTR_flag);
//            //execvp(token[0], token);
//
//        } else {
//            int wpid;
//            int status;
//            wpid = wait(&status);
//            if (wpid == -1) {
//                printf("\nParent (%d): wait(): %s\n", getpid(), strerror(errno));
//            } else {
//                //printf("parent process, waited wpid: %d with status:%d\n", wpid, status);
//            }
//        }
//        free_tokens(token);
//    }
//}

//if pid in pid list, return 1, else return 0;
int check_pid (int pid_list[], int pid, int cmdNum) {
    for(int i = 0; i <= cmdNum; i++) {
        printf("pid[%d]:%d\n",i, pid_list[i]);
    }
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
//        char**token = get_cmd();
        if (get_cmd(&cmds) == -1) {// if no command entered, print another prompt
            continue;
        }
        
        int n = 0;
        Cmd* c;
        //printf("cmdNum: %d\n", cmds.cmdNum);
        while (n <= (cmds.cmdNum)) {
            c = cmds.cmdArray[n];
            if( strcmp(c -> arg[0], "exit") == 0){       //printf("Goodbye!\n");
                exit(0);
            }
            pid = fork();
            sleep(1);
            if (pid == 0) {
                //make sure execvp did not unexpected fail
                printf("child process cmd[%d]\n", n);
                int EINTR_flag = 0;
                do {
                    EINTR_flag = 0;
                    if(execvp(c -> arg[0], c -> arg) == -1) {
        //                fprintf(stderr, "errno: %s\n", strerror( errno ) );
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
            n++;
        }
        //background or forground
        if (cmds.ifBackground) {
            //background, not waiting
            printf("background running\n");
            
        } else {
            //foreground, waiting
            int wpid_count = 0;
            while (wpid_count <= cmds.cmdNum) {
                sleep(1);
                int wpid;
                int status;
                printf("===parent waiting\n");
                wpid = wait(&status);
                if (check_pid(pid_list, wpid, cmds.cmdNum)) {
                    printf("wpid: %d\n", wpid);
                    wpid_count++;
                } else {
                    printf("zomie process: %d\n", wpid);
                }
                if (wpid == -1) {
                    printf("\nParent (%d): wait(): %s\n", getpid(), strerror(errno));
                } else {
                    printf("parent process, waited wpid: %d with status:%d\n", wpid, status);
                }
            }
            
            
        }

    }
}