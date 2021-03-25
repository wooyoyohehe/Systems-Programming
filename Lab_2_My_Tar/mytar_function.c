//
//  mytar_function.c
//  
//
//  Created by 龚晨 on 3/18/21.
//

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


void create_archive (const char* dir_name, char* arch_name) {
    static FILE *arch;
    struct stat buf;
    int exists;
    arch = fopen(arch_name, "a");
    //write magic number
    unsigned int magicnum = MAGICNUM;
    fwrite(&magicnum, 4, 1, arch);
//            printf("magic number set: %d\n\n", MAGICNUM);
    
    exists = lstat(dir_name, &buf);
    if (exists < 0) {
        fprintf(stderr, "%s not found，exists<0, lstat returns %d\n", dir_name);
    }
    //set inode, length, name, modification time
    unsigned long inode = buf.st_ino;
    unsigned int mode = buf.st_mode;
    unsigned int namesize = strlen(dir_name);
    time_t time = buf.st_mtime;
    fwrite (&inode, 8, 1, arch);
    fwrite (&namesize, 4, 1, arch);
    fwrite (dir_name, namesize, 1, arch);
    fwrite (&mode, 4, 1, arch); //mode
    fwrite (&time, 8, 1, arch); //time
    
    create_archive_helper(arch, dir_name);
    fclose(arch);

}


void create_archive_helper(FILE* arch, const char* dir_name){
    
    
    struct stat buf;
    int exists, total_size=0;
    struct dirent *de;
    char * fullname;
    DIR *d;
    
    d = opendir(dir_name);
    
    if (d == NULL) {
        fprintf(stderr, "Directory Failure %s\n", dir_name);
        exit(-1);
    }
    
    for (de = readdir(d); de != NULL; de = readdir(d)) {

        //combine two name to generate the path
        unsigned int namesize = strlen(dir_name) + strlen(de ->d_name) + 2; //added a "/" char and a null at the end of string.
        fullname = (char *)malloc(sizeof(char)*namesize);
        sprintf(fullname, "%s/%s", dir_name, de->d_name);
        
        
        if (strcmp(de->d_name, ".") ==0 || strcmp(de->d_name, "..") ==0 ) {
            continue;
        }

        exists = lstat(fullname, &buf);
        if (exists < 0) {
            fprintf(stderr, "%s not found，exists<0, lstat returns %d\n", fullname, exists);

        }
        //if not directory or regular file just skip it
        if(!S_ISDIR(buf.st_mode) && !S_ISREG(buf.st_mode))
            continue;
        //ignore . and ..

        if (S_ISLNK(buf.st_mode)){
            continue;
        }
        
        if( !get_inode( buf.st_ino ) ) {
            //inode not yet seen; add to list and process
            set_inode( buf.st_ino, fullname );
            //set inode, length, name, modification time
            unsigned long inode = buf.st_ino;
            unsigned int mode = buf.st_mode;
            int name_l = strlen(fullname);
            time_t time = buf.st_mtime;
            
            fwrite (&inode, 8, 1, arch);
            fwrite (&name_l, 4, 1, arch);
            fwrite (fullname, name_l, 1, arch);

            
            if (S_ISDIR(buf.st_mode)) { //directory
                fwrite (&mode, 4, 1, arch); //mode
                fwrite (&time, 8, 1, arch); //time
            } else {
                //is regualr file
                fwrite (&mode, 4, 1, arch); //mode
                fwrite (&time, 8, 1, arch); //time
                off_t size = buf.st_size;
                fwrite (&size, 8, 1, arch); //size
                //write content .... how?
                FILE* f = fopen(fullname, "r");
                if (f == NULL) {
                    perror("Error: can't open file. Writing file content\n");
                    exit(-1);
                }
                char* content = malloc(sizeof(char) * size);
                //creat a buff and read into it
                //what's the type of buff??
                fread(content, size, 1, f);
                fwrite(content, size, 1, arch);
                free(content);
                fclose(f);
                
            }
        } else {
            //inode seen just put number length and name
            
            unsigned long inode = buf.st_ino;
            fwrite (&inode, 8, 1, arch);
            int name_l = sizeof (fullname);
            fwrite (&namesize, 4, 1, arch);
            fwrite (fullname, namesize, 1, arch);
            
        }
        
        
        if (S_ISDIR(buf.st_mode) && strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 ) {
            create_archive_helper(arch, fullname);
        }
        
        
    }
    
    closedir(d);
    
}



void print_tar(char* arch_name) {
    //open tar
    FILE* tar = fopen(arch_name, "r");
    ino_t inode;
    int name_length;
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
    
    //read and prin
    while (!feof(tar)) {
        
        fread(&inode, 1, 8, tar);
        //only by having accessed the EOF can the feof know if it reaches the end.
        if (feof(tar))
            continue;

        fread(&name_length, 4, 1, tar);

        name = (char*)malloc(sizeof(char)*name_length);
        int n = fread(name, 1, name_length, tar);
        
        
        if (!get_inode(inode)) {
            mode_t mode;
            fread(&mode, 4, 1, tar);
            set_inode( inode, name );
            time_t mtime;
            fread(&mtime, 8, 1, tar);
            if (S_ISDIR(mode)) {
                //is directory
                printf("%s/ -- inode: %lu, mode: %o, mtime: %ld\n", name, inode, mode, mtime);
            } else if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                //executable file
                off_t size;
                fread (&size, 8, 1, tar);
                printf("%s* -- inode: %lu, mode: %o, mtime: %ld, size: %llu\n", name, inode, mode, mtime, size);
                //use fseek skip content
                if (fseek(tar, size, SEEK_CUR)) {
                    perror("ERROR: fseek failed\n");
                    exit(-1);
                }
            } else {
                //regular file
                off_t size;
                fread (&size, 8, 1, tar);
                printf("%s -- inode: %lu, mode: %o, mtime: %ld, size: %llu\n", name, inode, mode, mtime, size);
                if (fseek(tar, size, SEEK_CUR)) {
                    perror("ERROR: fseek failed\n");
                    exit(-1);
                }
            }
        
        } else {
            //hard link, inode has been seen
            printf("%s -- inode: %lu\n", name, inode);
        
        }
        
    }
    
    fclose(tar);
}






