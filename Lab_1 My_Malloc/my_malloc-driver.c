//
//  my_malloc-driver.c
//  Lab1: Malloc
//


#include "my_malloc.h"
#include <stdio.h>




int main(int argc, const char * argv[])
{
    
    int* A = my_malloc(4*sizeof(int));
    
    *A = 1;
    *(A+1) = 2;
    *(A+2) = 3;
    *(A+3) = 4;
    
    for(int i = 0; i < 4; i++)
    printf("A[%d] = %d\n", i, *(A+i));
    
    char* B = my_malloc(8193);
    *(B+8192) = 'c';
    printf("B[8193] = %c\n", *(B+8192));
    
    my_free(A);
    printf("A freed\n");
    my_free(B);
    
    return 0;
}
