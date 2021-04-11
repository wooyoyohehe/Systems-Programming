//
//  test.c
//  
//
//  Created by 龚晨 on 4/9/21.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
char* prompt = "mysh: ";

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
int main (int argc, char* argv[]) {
    
    //check if prompt offered, no prompt, will print mysh:
    //set_prompt(argv[]);
    
    //printf("%d  %s\n", argc, argv[1]);
    set_prompt(argc, argv);
    printf("%s", prompt);
    
    char a[1024];
    
//    char c = fgetc(0);
//    int i = 0;
//    while (!feof(0)) {
//
//        a[i++] = c;
//        c = fgetc(0);
//
//    }
    
    char line[1024];
    if(!fgets(line, 1024, stdin))
        printf("fgets failed\n");
    
    printf("%s\n", line);
    
}
