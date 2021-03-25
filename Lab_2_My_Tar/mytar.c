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
    
//    struct stat buf;
//
//    int exists = stat(option.dir_name, &buf);
    
    
    return option;
}



int main(int argc, char *argv[]) {
    Option option = read_options(argc, argv);
    
//    printf("%c\n", option.mode);
//    printf("%s\n", option.dir_name);
//    printf("%s\n", option.arch_name);
    
    switch (option.mode) {
        case 'c':
            //test
//            get_size(option.dir_name);
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


//int get_size( const char * dirname ){
//    struct stat buf;
//    int exists, total_size=0;
//    DIR *d;
//    struct dirent *de;
//    char * fullname;
//
//    d = opendir(dirname);
//    if (d == NULL) {
//        fprintf(stderr, "Couldn't open %s\n", dirname);
//        exit(1);
//    }
//
//    fullname = (char *)malloc(sizeof(char)*(strlen(dirname)+258));
//
//    for (de = readdir(d); de != NULL; de = readdir(d)) {
//        sprintf(fullname, "%s/%s", dirname, de->d_name);
//
//        exists = stat(fullname, &buf);
//        if (exists < 0) {
//            fprintf(stderr, "%s not found\n", fullname);
//        }
//
//        if( !get_inode( buf.st_ino ) ) {
//            //inode not yet seen; add to list and process
//            set_inode( buf.st_ino, fullname );
//
//            if (S_ISDIR(buf.st_mode)) {
//                printf("%10lld %s/\n", buf.st_size, fullname);
//            } else if (S_ISLNK(buf.st_mode)) {
//                printf("%10lld %s@\n", buf.st_size, fullname);
//            } else if (buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
//                printf("%10lld %s*\n", buf.st_size, fullname);
//            } else {
//                printf("%10lld %s\n", buf.st_size, fullname );
//            }
//            total_size+=buf.st_size;
//        }
//
//        /* Make the recursive call if the file is a directory */
//        if (S_ISDIR(buf.st_mode) && strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 ) {
//            total_size += get_size(fullname);
//        }
//    }
//
//    closedir(d);
//    return total_size;
//}



