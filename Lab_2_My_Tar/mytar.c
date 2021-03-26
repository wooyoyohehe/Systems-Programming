//
//  mytar.c
//  
//
//  Created by 龚晨 on 3/18/21.
//

#include "mytar.h"
//#include "mytar_function"
#include "inodemap.h"
#include "mytar_function.h"
#include "extract_tar.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/time.h>

#define MAGICNUM 0x7261746D

typedef struct
{
    char mode;
    char* arch_name;
    char* dir_name;
}Option;


Option read_options(int argc, char* argv[]) {
    Option option;
    int f_find = 0;
    int op_find = 0;
    int o;
    while( (o=getopt(argc,argv,"cxtf:")) != -1){
        switch (o) {
            case 'c':
                if (op_find) {
                    fprintf(stderr, "Error: Multiple modes specified.\n");
                    exit(-1);
                }
                op_find = 1;
                option.mode = 'c';
                break;
            case 'x':
                if (op_find) {
                    fprintf(stderr, "Error: Multiple modes specified.\n");
                    exit(-1);
                }
                op_find = 1;
                option.mode = 'x';
                break;
            case 't':
                if (op_find) {
                    fprintf(stderr, "Error: Multiple modes specified.\n");
                    exit(-1);
                }
                op_find = 1;
                option.mode = 't';
                break;
            case 'f':
                f_find = 1;
//                printf("%s\n", optarg);
                option.arch_name = optarg;
                break;
            default:
                break;
                
        }
        
    }
    
    if (!op_find) {
        fprintf(stderr, "Error: No mode specified.\n");
        exit(-1);
    }
    
    if (!f_find) {
        fprintf(stderr, "Error: No tarfile specified.\n");
        exit(-1);
    }
    
    //set option 'c' directory
    if (option.mode == 'c') {
        if(optind >= argc){
            fprintf(stderr,"Error: No directory target specified.\n");
            exit(-1);
        }
        option.dir_name = argv[optind];
    }
    
    
    return option;
}



int main(int argc, char *argv[]) {
    Option option = read_options(argc, argv);
    
    switch (option.mode) {
        case 'c':
            create_archive(option.dir_name, option.arch_name);
            break;
        case 't':
            print_tar(option.arch_name);
            break;
        case 'x':
            extract(option.arch_name);
    }
    
    return 0;
}


