//
//  test.c
//  
//
//  Created by 龚晨 on 5/5/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>



void *thread_print(int n){
    int* i;
    i = malloc(sizeof(int));
    *i = n;
    printf("thread:n, i = %d\n", *i);
    printf("thread:n, &i = %d\n", i);
    
}

int main(){
    pthread_t tid;
    int*i;
    int a = 1;
    i = &a;
    if (pthread_create(&tid, NULL, thread_print, *i)) {
      printf("Pthread create failed\n");
      exit(1);
    }
    sleep(2);
    *i = 2;
    thread_print(*i);
    
    
    return 0;
}
