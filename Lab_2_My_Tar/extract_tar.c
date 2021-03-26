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
#include <unistd.h>
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
        exit(-1);
    }
    
    //check magic num
    unsigned int magic;
    fread(&magic, 4, 1, tar);
    if (magic != MAGICNUM) {
        fprintf(stderr, "Error: Bad magic number (%d), should be: %d.\n", magic, MAGICNUM);
        exit(-1);
    }
    
    
//==========begin to read the tar file by iteration===========
    while (!feof(tar)) {
        //printf("\n- - - - - - - - - - - - - - -\n");
//        unsigned int name_length;
//        char* name;
        fread(&inode, 8, 1, tar);
        
        //only by having accessed the EOF can the feof know if it reaches the end.
        if (feof(tar))
            continue;
        
        fread(&name_length, 4, 1, tar);
        
        //printf("name_length: %d\n", name_length);
        name = (char*) malloc (name_length + 1);

        fread(name, 1, name_length, tar);
        name[name_length] = '\0';
//        printf("NAME: %s   name_length: %d  read_length:%lu\n", name, name_length, strlen(name));
        
//        for (int i = 0; i <= name_length; i++) {
//            printf("[%d]: %c ", i, name[i]);
//        }
//        if (name[name_length] == '\0')
//            printf("is null");
//
//        printf("\n");
//
        
        
        
        if (!get_inode(inode)) {
            
            mode_t mode;
            fread(&mode, 4, 1, tar);
            //printf("name: %s\n", name);
//            printf("Set InodeMap: inode: %llu NAME: %s\n", inode, name);
            
            set_inode( inode, name );

//            printf("get_inode right after set: inode: %llu NAME: %s\n", inode, get_inode(inode));
            
//            printf("Before Setting Time\n");
            
            //set time
            struct timeval mtime;
            struct timeval currtime;
            
            fread(&(mtime.tv_sec), 8, 1, tar);
            mtime.tv_usec = 0;
            gettimeofday (&currtime, NULL);
            currtime.tv_usec = 0;
            struct timeval times[2];
            times[0] = mtime;
            times[1] = currtime;
//            printf("currtime: %ld\n", times[1].tv_sec);
            
            if (S_ISDIR(mode)) {
                //is directory
//                printf("%s/ -- inode: %lu, mode: %o, mtime: %ld\n", name, inode, mode, mtime);
                if (mkdir(name, mode)) {
                    perror("mkdir failed\n");
                    exit(-1);
                    //printf("dir_name: %s\n", name);
                }
                utimes(name, times);
//                chmod(name, mode);

                
            } else { //regular file and executable
                FILE* f = fopen(name, "w");
                chmod(name, mode);
                off_t size;
                fread (&size, 8, 1, tar);
//                printf("%s -- inode: %lu, mode: %o, mtime: %ld, size: %llu\n", name, inode, mode, mtime, size);
                
                //writing content
                char* content = malloc(sizeof(char) * size);
                fread (content, size, 1, tar);
                fwrite(content, size, 1, f);
                
                gettimeofday (&currtime, NULL);
                currtime.tv_usec = 0;
                times[1] = currtime;
                
                utimes(name, times); //time
                
                free(content);
                fclose(f);
            }

            
        } else {
            //hard link, inode has been seen
//            printf("GET_Inode returns: inode: %llu %s\n", inode, get_inode(inode));
            const char* prfile = get_inode(inode);
            if (link(prfile, name)) {  //create hard link to the previous file
                perror("hard link create failed: \n");
                exit(-1);
//                printf("inode: %llu PreFile name: %s   File name: %s\n", inode, prfile, name);
            }
//            printf("%s -- inode: %lu\n", name, inode);
            
        }
    }
//=================End of the While loop==============
    
    
    
    
}
