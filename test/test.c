/* Adaptation of ls demo from James S. Plank, CS360 -- Spring, 1994 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define MAPSIZE 1024
#define MAGICNUM 0x7261746D
char ** Map=NULL;

void set_inode( ino_t i, char * f ){
    uint32_t mappos = i % MAPSIZE;
    
    if( Map == NULL )
        Map = (char  **)calloc(MAPSIZE, sizeof(char*));
    
    Map[mappos] = f;
}

char * get_inode( ino_t i ){
    if ( Map == NULL)
        return 0;
    
    uint32_t mappos = i % MAPSIZE;
    return Map[ mappos ];
}

int get_size( const char * dirname ){
    struct stat buf;
    int exists, total_size=0;
    DIR *d;
    struct dirent *de;
    char * fullname;

    d = opendir(dirname);
    if (d == NULL) {
        fprintf(stderr, "Couldn't open %s\n", dirname);
        exit(1);
    }

    fullname = (char *)malloc(sizeof(char)*(strlen(dirname)+258));
    
    for (de = readdir(d); de != NULL; de = readdir(d)) {
        sprintf(fullname, "%s/%s", dirname, de->d_name);

        exists = stat(fullname, &buf);
        if (exists < 0) {
            fprintf(stderr, "%s not found\n", fullname);
        }
        
        if( !get_inode( buf.st_ino ) ) {
            //inode not yet seen; add to list and process
            set_inode( buf.st_ino, fullname );
            
            if (S_ISDIR(buf.st_mode)) {
                printf("%10lld %s/\n", buf.st_size, fullname);
            } else if (S_ISLNK(buf.st_mode)) {
                printf("%10lld %s@\n", buf.st_size, fullname);
            } else if (buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                printf("%10lld %s*\n", buf.st_size, fullname);
            } else {
                printf("%10lld %s\n", buf.st_size, fullname );
            }
            total_size+=buf.st_size;
        }
        
        /* Make the recursive call if the file is a directory */
        if (S_ISDIR(buf.st_mode) && strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 ) {
            total_size += get_size(fullname);
        }
    }
    
    closedir(d);
    return total_size;
}

//int main(int argc, char **argv)
//{
//
////    printf("%10d total\n", get_size(".") );
//
////    FILE* fp;
////    fp = fopen("testOpen.txt", "a+");
////    int a = 35;
////    char* str = "abcc\0";
////    fwrite(&a, 4,1,fp);
////    fwrite(str, 5, 1, fp);
////    fclose(fp);
////
////
//
//
//
//
//    struct stat buf;
//    int exists, total_size=0;
//
//    struct dirent *de;
//    char * fullname;
//    DIR *d;
//    static FILE *arch;
//    static int magicSet = 0;
//
//    char* dir_name = "testDir1\0";
//
//
//    d = opendir(dir_name);
//
//    if (d == NULL) {
//        fprintf(stderr, "Directory Failure %s\n", dir_name);
//        exit(-1);
//    }
//
//
//        arch = fopen("testDir.txt", "a");
//
//
//    fullname = (char *)malloc(sizeof(char)*(strlen(dir_name)+258));
//
//    for (de = readdir(d); de != NULL; de = readdir(d)) {
//        //set magic
//        printf("name: %s,\n", de ->d_name);
//
//        if (!magicSet) {
//            magicSet = 1;
//            int magicnum = 2021;
//            fwrite(&magicnum, 4, 1, arch);
//        }
//
//        sprintf(fullname, "%s/%s", dir_name, de->d_name);
//
//        exists = stat(fullname, &buf);
//        if (exists < 0) {
//            fprintf(stderr, "%s not found，exists<0\n", fullname);
//        }
//
//        printf("inode: %llu\n", buf.st_ino);
//        time_t time = buf.st_mtime;
//        printf("Mtime: %ld\n\n", time);
//            //inode seen just put number length and name
//            unsigned long inode = buf.st_ino;
//            fwrite (&inode, 8, 1, arch);
//            int name_l = sizeof (fullname);
//            fwrite (&name_l, 4, 1, arch);
//            fwrite (fullname, name_l, 1, arch);
//
//        }
//
//    closedir(d);
//    fclose(arch);
//    return 0;
//}
void creat_archive (char* dir_name, char* arch_name);
void print_tar(char* arch_name);

int main(int argc, char **argv) {
//
//    char* dir_name = "testDir1\0";
//    char* arch_name = "testDir1.txt\0";
//    creat_archive (dir_name, arch_name);

    char* arch_name = "testDir1.txt\0";
    print_tar(arch_name);
    return 0;
}

void creat_archive (char* dir_name, char* arch_name) {
    
    struct stat buf;
    int exists, total_size=0;

    struct dirent *de;
    char * fullname;
    DIR *d;
    static FILE *arch;
    static int magicSet = 0;
    
    
    d = opendir(dir_name);
    if (d == NULL) {
        fprintf(stderr, "Directory Failure %s\n", dir_name);
        exit(-1);
    }
    
    if (arch == NULL) {
        arch = fopen(arch_name, "a");
    }

    //set magic and write the main directory
    if (!magicSet) {
        magicSet = 1;
        unsigned int magicnum = MAGICNUM;
        fwrite(&magicnum, 4, 1, arch);
//            printf("magic number set: %d\n\n", MAGICNUM);
        
        exists = lstat(dir_name, &buf);
        if (exists < 0) {
            fprintf(stderr, "%s not found，exists<0\n", dir_name);
        }
        //set inode, length, name, modification time
        unsigned long inode = buf.st_ino;
        unsigned int mode = buf.st_mode;
        int namesize = strlen(dir_name);
        time_t time = buf.st_mtime;
        
        fwrite (&inode, 8, 1, arch);
//        printf("inode is: %ld\n", inode);
        fwrite (&namesize, 4, 1, arch);
        fwrite (dir_name, namesize, 1, arch);
        fwrite (&mode, 4, 1, arch); //mode
//        printf("mode is: %o\n", mode);
        fwrite (&time, 8, 1, arch); //time
        
    }
    
    
    
    for (de = readdir(d); de != NULL; de = readdir(d)) {

        unsigned int namesize = strlen(dir_name) + strlen(de ->d_name) + 1; //added a "/" char
        fullname = (char *)malloc(sizeof(char)*namesize);
        sprintf(fullname, "%s/%s", dir_name, de->d_name);
        

        exists = lstat(fullname, &buf);
        if (exists < 0) {
            fprintf(stderr, "%s not found，exists<0\n", fullname);
        }

        if (S_ISLNK(buf.st_mode)){
            continue;
        }
        //if not directory or regular file just skip it
        if(!S_ISDIR(buf.st_mode) && !S_ISREG(buf.st_mode))
            continue;
        //ignore . and ..
        if (strcmp(de->d_name, ".") ==0 || strcmp(de->d_name, "..") ==0 ) {
//            printf("is . or .. skipped\n\n");
            continue;
        }
        

        
        if( !get_inode( buf.st_ino ) ) {
            //inode not yet seen; add to list and process
            set_inode( buf.st_ino, fullname );
            //set inode, length, name, modification time
            unsigned long inode = buf.st_ino;
            unsigned int mode = buf.st_mode;
//            int name_l = sizeof (fullname);
            time_t time = buf.st_mtime;
            
            fwrite (&inode, 8, 1, arch);
            printf("inode is: %ld\n", inode);
            fwrite (&namesize, 4, 1, arch);
            fwrite (fullname, namesize, 1, arch);
            printf("fullname is: %s\n", fullname);
            printf("set namesize: %u\n", namesize );
            
            if (S_ISDIR(buf.st_mode)) { //directory
                fwrite (&mode, 4, 1, arch); //mode
                printf("mode is: %o\n", mode);
                fwrite (&time, 8, 1, arch); //time
            } else {
                //is regualr file
                fwrite (&mode, 4, 1, arch); //mode
                printf("mode is: %o\n", mode);
                fwrite (&time, 8, 1, arch); //time
                off_t size = buf.st_size;
                fwrite (&size, 8, 1, arch); //size
                //write content .... how?
                FILE* f = fopen(fullname, "r");
                char* content = malloc(sizeof(char) * size);
                //creat a buff and read into it
                //what's the type of buff??
                fread(content, size, 1, f);
                fwrite(content, size, 1, arch);
                fclose(f);
            }
            
        } else {
            //inode seen just put number length and name
            unsigned long inode = buf.st_ino;
            fwrite (&inode, 8, 1, arch);
 
            fwrite (&namesize, 4, 1, arch);
            fwrite (fullname, namesize, 1, arch);
            
        }
        
        if (S_ISDIR(buf.st_mode) && strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 ) {
            creat_archive(fullname, arch_name);
        }
        
        
    }
    
    closedir(d);
    
}


void print_tar(char* arch_name) {
    //open tar
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
    printf("magicnum: %d\n", magic);
    
    //read and prin
    while (!feof(tar)) {
        fread(&inode, 8, 1, tar);
        fread(&name_length, 4, 1, tar);
        printf("name_length: %u\n", name_length);
        name = (char*)malloc(sizeof(char)*name_length);
        fread(name, name_length, 1, tar);
        printf("name: %s\n", name);
        
        mode_t mode;
        fread(&mode, 4, 1, tar);
        
        if (!get_inode(inode)) {
            set_inode( inode, name);
            time_t mtime;
            fread(&mtime, 8, 1, tar);
            printf("mode: %o\n", mode);
            if (S_ISDIR(mode)) {
                //is directory
                printf("fileType: Dir:\n");
                printf("%s/ -- inode: %llu, mode: %o, mtime: %ld\n\n", name, inode, mode, mtime);
            } else if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                //executable file
                off_t size;
                fread (&size, 8, 1, tar);
                printf("%s* -- inode: %llu, mode: %o, mtime: %ld, size: %llu\n\n", name, inode, mode, mtime, size);
                //use fseek skip content
                if (fseek(tar, size, SEEK_CUR)) {
                    perror("ERROR: fseek failed\n");
                    exit(-1);
                }
            } else {
                //regular file
                printf("regular file\n");
                off_t size;
                fread (&size, 8, 1, tar);
                printf("%s -- inode: %llu, mode: %o, mtime: %ld, size: %llu\n\n", name, inode, mode, mtime, size);
                if (fseek(tar, size, SEEK_CUR)) {
                    perror("ERROR: fseek failed\n");
                    exit(-1);
                }
            }
        
        } else {
            //hard link, inode has been seen
            printf("%s -- inode: %llu\n", name, inode);
        
        }
        
    }
    
    fclose(tar);
}
