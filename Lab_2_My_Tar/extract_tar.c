//
//  extract_tar.c
//  
//
//  Created by 龚晨 on 3/21/21.
//

#include "extract_tar.h"
#include "mytar_function.h"
#include "mytar.h"
//#include "mytar_function"
#include "inodemap.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/time.h>

#define MAGICNUM 0x7261746D


void extract (char* arch_name) {
    
    FILE* tar = fopen(arch_name, "r");
    ino_t inode;
    unsigned int name_length;
    char* name;
    
    if (tar == NULL) {
        perror("Error: open tar failed");
    }
    
    //check magic num
    unsigned int magic;
    fread(&magic, 4, 1, tar);
    if (magic != MAGICNUM) {
        fprintf(stderr, "Error: Bad magic number (%d), should be: %d.\n", magic, MAGICNUM);
        exit(-1);
    }
    
    while (!feof(tar)) {
        fread(&inode, 8, 1, tar);
        
        fread(&name_length, 4, 1, tar);

        name = (char*)malloc(sizeof(char)*name_length);
        fread(name, name_length, 1, tar);
        
        
        if (!get_inode(inode)) {
            mode_t mode;
            fread(&mode, 4, 1, tar);
            set_inode( inode, name );
            
//            struct timeval times[2];
//            times[0] = malloc(sizeof(struct timeval));
//            times[1] = malloc(sizeof(struct timeval));
//            time_t mtime;
//            time_t currtime;
//            fread(&mtime, 8, 1, tar);
//            gettimeofday (&currtime, NULL);
//            times[0].tv_sec = mtime;
//            times[0].tv_usec = 0;
//            times[1].tv_sec = currtime;
//            times[1].tv_usec = 0;
            
            struct timeval mtime;
            struct timeval currtime;
            fread(&(mtime.tv_sec), 8, 1, tar);
            mtime.tv_usec = 0;
            gettimeofday (&(currtime.tv_sec), NULL);
            currtime.tv_usec = 0;
            struct timeval times[2];
            times[0] = mtime;
            times[1] = currtime;
            
            if (S_ISDIR(mode)) {
                //is directory
                printf("%s/ -- inode: %lu, mode: %o, mtime: %ld\n", name, inode, mode, mtime);
                if (mkdir(name, mode)) {
                    perror("mkdir failed \n");
                }
                utimes(name, times);
//                chmod(name, mode);
                
            }  else {
                //regular file and executable
                FILE* f = fopen(name, "w");
                chmod(name, mode);
                utimes(name, times); //time
                off_t size;
                fread (&size, 8, 1, tar);
                printf("%s -- inode: %lu, mode: %o, mtime: %ld, size: %llu\n", name, inode, mode, mtime, size);
                
                //writing content
                char* content = malloc(sizeof(char) * size);
                fread (content, size, 1, tar);
                fwrite(content, size, 1, f);
                free(content);
                fclose(f);
                
//                if (fseek(tar, size, SEEK_CUR)) {
//                    perror("ERROR: fseek failed\n");
//                    exit(-1);
//                }
            }

            
        } else {
            //hard link, inode has been seen
            char* prfile = get_inode(inode);
            if (link(prfile, name))  //create hard link to the previous file
                perror("hard link create failed\n");
            printf("%s -- inode: %lu\n", name, inode);
        
        }
        
    }
    
    
    
    
    
    
    
    
}
