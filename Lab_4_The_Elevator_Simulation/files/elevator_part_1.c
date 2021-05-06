//
//  elevator_part_1.c
//  
//
//  Created by Chen Gong
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "dllist.h"
#include "elevator.h"

#define talloc(ty, sz) (ty *) malloc ((sz) * sizeof(ty))
//pthread_cond_t p_ready;
//pthread_cond_t e_ready;
Dllist people;
pthread_cond_t e;


void initialize_simulation(Elevator_Simulation *es)
{
    //set up the global list and a condition variable for blocking elevators.
    printf("init simulation in\n");
    pthread_mutex_lock(es->lock);
    people = new_dllist();
    es->v = talloc(pthread_cond_t, 1);
    pthread_mutex_unlock(es->lock);
    printf("init simulation out\n");
}

void initialize_elevator(Elevator *e)
{
    
}

void initialize_person(Person *e)
{
}

void wait_for_elevator(Person *p)
{
    //append the person to the global list. Signal the condition variable for blocking elevators. Block on the person’s condition variable.
    printf("wait for elevator in\n");
    pthread_mutex_lock(p->es->lock);
    dll_append(people, new_jval_v((void *) p));
    pthread_mutex_unlock(p->es->lock);
    
    pthread_mutex_lock(p->lock);
    pthread_cond_signal(p->es->v);
    pthread_cond_wait(p->cond, p->lock);
    
    pthread_mutex_unlock(p->lock);
    printf("wait for elevator out\n");
    
}

void wait_to_get_off_elevator(Person *p)
{
    //Unblock the elevator’s condition variable and block on the person’s condition variable.
    pthread_mutex_lock(p->lock);
    pthread_cond_signal(p->e->cond);
    pthread_cond_wait(p->cond, p->lock);
    pthread_mutex_unlock(p->lock);
}

void person_done(Person *p)
{
    pthread_mutex_lock(p->lock);
    pthread_cond_signal(p->e->cond);
    pthread_mutex_unlock(p->lock);
    
}

void *elevator(void *arg)
{
    /*1. Check the global list and if it’s empty, block on the condition variable for blocking elevators.
      2. put itself into the person's e field.
     3.move to the person's floor, open door
     4.signal person and wait
     5.When it wakes up, it goes to the person’s destination floor, opens its door, signals the person and blocks.
     6. When the person wakes it up, it closes its door and re-executes its while loop.*/
    
    Elevator *e;
    e = (Elevator*) arg;
    printf("enter elevator\n");
    while(1) {
        pthread_mutex_lock(e->es->lock);
        printf("before while\n");
        while(dll_empty(people)) //wait if empty;
        {
            printf("before wait\n");
            pthread_cond_wait(e->es->v, e->es->lock);
            printf("waited\n");
        }
            
        printf("after while\n");
        
        Dllist tmp = dll_first(people);
        printf("1\n");
        Person* p = (Person*)jval_v(tmp ->val);//get the first person
        dll_delete_node(tmp); //delete the person from the global list
        printf("%s\n", p->fname);
        p->e = e;
        printf("2\n");
        pthread_mutex_unlock(e->es->lock);
        
        pthread_mutex_lock(e->lock);
        move_to_floor(e, p->from);
        open_door(e);
        pthread_cond_signal(p->cond);
        
        pthread_cond_wait(e->cond, e->lock);
        close_door(e);
        move_to_floor(e, p->to);
        open_door(e);
        pthread_cond_signal(p->cond);
        pthread_cond_wait(e->cond, e->lock);
        close_door(e);
        //where should I use the cond in elevator?;
        pthread_mutex_unlock(e->lock);
    }
    
  return NULL;
}
