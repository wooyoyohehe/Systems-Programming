//
//  elevator_part_2.c
//  
//
//  Created by Chen Gong 
//

#include <stdio.h>
#include <pthread.h>
#include "elevator.h"
#include "dllist.h"


#define dll_traverse(ptr, list) \
for (ptr = list->flink; ptr != list; ptr = ptr->flink)
//traverse list, l, with node, ptr, for each iteration
#define dll_rtraverse(ptr, list) \
for (ptr = list->blink; ptr != list; ptr = ptr->blink)
//traverse list, l, in reverse with node, ptr, for each iteration

#define talloc(ty, sz) (ty *) malloc ((sz) * sizeof(ty))
Dllist people;
pthread_cond_t e_hold;


void initialize_simulation(Elevator_Simulation *es)
{
    //set up the global list and a condition variable for blocking elevators.
    //pthread_mutex_lock(es->lock);
    people = new_dllist();
    //es->v = talloc(pthread_cond_t, 1);
    //e_hold = talloc(pthread_cond_t, 1);
    pthread_cond_init(&e_hold, NULL);
    //pthread_mutex_unlock(es->lock);
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
    pthread_mutex_lock(p->es->lock);
    dll_append(people, new_jval_v((void *) p));
    pthread_mutex_unlock(p->es->lock);
    
    pthread_mutex_lock(p->lock);
    pthread_cond_signal(&e_hold);
    pthread_cond_wait(p->cond, p->lock);
    
    pthread_mutex_unlock(p->lock);
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

void loading_list(Dllist l, Elevator* e, int direction) {
    //return the people need on this floor in the same direction and remove them from the global list.
    pthread_mutex_lock(e->es->lock);
    Dllist ptr;
    Dllist tmp;
    Person* p;
    for (ptr = people->flink; ptr != people; ptr = ptr->flink) {//marco
        p = (Person*)(jval_v(ptr->val));
        if(p->from == e->onfloor && (p->to - e->onfloor)*direction > 0){ //same direction
            dll_append(l, new_jval_v((void *) p));
            tmp = ptr;
            ptr = ptr->blink; //set p to its previous node, so that it can iterate correctly to the next one.
            dll_delete_node(tmp);
        }
    }
    pthread_mutex_unlock(e->es->lock);
}

void unloading_list(Dllist l, Elevator* e) {
    Dllist ptr;
    Person* p;
    for (ptr = e->people->flink; ptr != e->people; ptr = ptr->flink) {//marco
        p = (Person*)(jval_v(ptr->val));
        if(p->to == e->onfloor){
            dll_append(l, new_jval_v((void *) p));
              //deletion will be done in get off elevator
        }
    }
}

void *elevator(void *arg)
{
    Elevator *e;
    e = (Elevator*) arg;
    int direction = 1; //1 up -1 down
    Dllist loading = new_dllist();
    Dllist unloading = new_dllist();
    Dllist ptr;
    Dllist tmp;
    Person* p;
    
    while(1) {
        //marco, for loop
        loading_list(loading, e, direction);
        unloading_list(unloading, e);
        
        ptr = dll_first(unloading);
        while(!dll_empty(unloading)){ //unload people
            if(!e->door_open){
                open_door(e);
            }
            p = (Person*)jval_v(ptr->val);
            pthread_cond_signal(p->cond);
            
            //wait for person leave
            pthread_mutex_lock(e->lock);
            pthread_cond_wait(e->cond, e->lock);
            pthread_mutex_unlock(e->lock);
            
            tmp = ptr;
            ptr = ptr->flink;
            dll_delete_node(tmp);
        }
        
        ptr = dll_first(loading);
        while(!dll_empty(loading)){ // loading people
            if(!e->door_open){
                open_door(e);
            }
            p = (Person*)jval_v(ptr->val);
            pthread_mutex_lock(p->lock);
            p->e = e; //set person's elevator
            pthread_cond_signal(p->cond); //notify the person
            pthread_mutex_unlock(p->lock);
            
            pthread_mutex_lock(e->lock);
            pthread_cond_wait(e->cond, e->lock); //wait for the person get in the elevator
            pthread_mutex_unlock(e->lock);
            
            tmp = ptr;
            ptr = ptr->flink;
            dll_delete_node(tmp);
        }

              
          if (e->door_open) {
              close_door(e);
          }
              
        if(direction == 1){
            move_to_floor(e, e->onfloor + 1);
            if (e->onfloor == e->es->nfloors)
                direction = -1;
        } else {
            move_to_floor(e, e->onfloor - 1);
            if (e->onfloor == 1)
                direction = 1;
        }
        
    }
    
    
  return NULL;
}
