//
//  my_malloc-driver.c
//  Lab1: Malloc
//


#include "my_malloc.h"
#include <stdio.h>
#include <stdlib.h>

void checkLarge_number();

void print_header(void*ptr);

void check_header();

int main(int argc, const char * argv[])
{
    check_header();
    
//    int* A = my_malloc(4*sizeof(int));
//
//    *A = 1;
//    *(A+1) = 2;
//    *(A+2) = 3;
//    *(A+3) = 4;
//
//    int* i = (int*)A - 2;
//    int* magic = (int*)A - 1;
//    printf("Header of A: %d  %d\n", *i, *magic);
//    for(int i = 0; i < 4; i++){
//        printf("A[%d] = %d\n", i, *(A+i));
//    }
    
    //+++++++
//    int* Array[3];
//    int mul = 9;
//    for(int j = 0; j < 3; j ++) {
//        printf("\n\n %d =====\n", j);
//        Array[j] = my_malloc(3 * sizeof(int));
////        mul *= mul;
////        printf("freeList after %d malloc:\n", j+1);
////        helper_print_freeList();
//    }
//
//    for(int i = 0; i < 3; i++){
//        for(int j = 0; j < 3; j++){
//            Array[i][j] = 3;
//        }
//
//    }
//
//    for(int i = 0; i < 3; i++){
//        for(int j = 0; j < 3; j++){
//            printf(" %d", Array[i][j]);
//        }
//        printf("\n");
//    }

//    for(int j = 0; j < 3; j ++) {
//        int* i = (int*)Array[j] - 2;
//        void* ptr = (void*)((int*)Array[j] - 2);
//        int* magic = (int*)Array[j] - 1;
//        printf("Header of Array[%d]: %d  %d\n", j , *i, *magic);
//        printf("Address of header: %p\n", ptr);
//        printf("Address of Array[j][0]: %p\n", &Array[j][0]);
//        printf("Array[%d][0] = %d\n", j, Array[j][0]);
//
//    }

//    int* Array[3];
//    int mul = 9;
//    for(int j = 0; j < 3; j ++) {
//        Array[j] = malloc(3 * sizeof(int));
//        mul *= mul;
//    }
//    for(int j = 0; j < 3; j ++) {
//        int i = (int)Array[j][0] - 2;
//        int magic = (int)Array[j][0] - 1;
//        printf("Header of Array[%d]: %d  %d\n", j , i, magic);
//    }
//
    
    
    char* B = my_malloc(8120);
    *(B+8119) = 'c';
    printf("B[7999] = %c\n", *(B+8119));
    

    
    helper_print_freeList();
    
    my_free(B);
    printf("B free\n");
    
    helper_print_freeList();

    return 0;
}


void checkLarge_number(){

    char* B = my_malloc(8193);
    *(B+8192) = 'c';
    printf("B[8193] = %c\n", *(B+8192));

    
}

void check_header(){
    int* Array[3];
    int mul = 9;
    for(int j = 0; j < 3; j ++) {
        printf("\n\n %d =====\n", j);
        Array[j] = my_malloc(3 * sizeof(int));
        print_header(Array[j]);
//        mul *= mul;
//        printf("freeList after %d malloc:\n", j+1);
//        helper_print_freeList();
    }
    
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            Array[i][j] = 3;
        }

    }

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            printf(" %d", Array[i][j]);
        }
        printf("\n");
    }
    helper_print_freeList();
    my_free(Array[1]);
    
}

void print_header(void*ptr){
    printf("Address of header begin: %p\n", (char*)ptr - 8);
    printf("Address of chunk begin: %p\n", ptr);
}
