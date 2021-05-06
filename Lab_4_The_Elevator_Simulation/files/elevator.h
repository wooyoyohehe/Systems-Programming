/* elevator.h
   Header file for the elevator threads lab.
   Jim Plank
   CS560
   Lab 2
   January, 2009
 */

/* Main elevator struct */

#include <pthread.h>
#include "dllist.h"

typedef struct {
  int nfloors; 
  int nelevators;
  double interarrival_time;   /* The interarrival time of people */
  double door_time;           /* The time to open or close a door */
  double floor_to_floor_time; /* The time to go from one floor to an adjacent floor */
  int npeople_started;        /* Stats. */
  int npeople_finished;
  pthread_mutex_t *lock;
  void *v;                    /* This is what you get to define */
} Elevator_Simulation;

typedef struct {
  int id;             /* Elevator id */
  int onfloor;        /* What floor the elevator is currently on */
  int door_open;      /* Whether the door is open */
  int moving;         /* Whether the elevator is moving */
  Dllist people;      /* Dllist of people on the elevator */
  pthread_mutex_t *lock;    
  pthread_cond_t *cond;    
  void *v;
  Elevator_Simulation *es;
} Elevator;

typedef struct {
  char *fname;           /* Person's first name */
  char *lname;           /* Person's last name */
  int from;              /* Starting floor */
  int to;                /* Ending floor */
  double arrival_time;   /* When the person will arrive at "from" */
  Elevator *e;           /* The person's elevator */
  Dllist ptr;            /* Pointer to the person's entry on the elevator's dllist */
  pthread_mutex_t *lock;    
  pthread_cond_t *cond;    
  void *v;
  Elevator_Simulation *es;
} Person;


/* ------------------------------------------------------------------ */
/* Procedures Defined in elevator_skeleton.c */

extern void move_to_floor(Elevator *e, int floor);     /* Blocks until the elevator is at the given floor. */

extern void open_door(Elevator *e);                    /* Blocks until the elevator's door is open. */

extern void close_door(Elevator *e);                   /* Blocks until the elevator's door is closed. */

extern void get_on_elevator(Person *p);               /* Called when a person gets on an elevator.  The elevator
                                                       must be on the person's "from" floor with the door open. 
                                                       This will add the person to the elevator's list (setting
                                                       the person's ptr appropriately. */

extern void get_off_elevator(Person *p);              /* Called when a person gets off an elevator.  The elevator
                                                       must be on the person's "to" floor with the door open,
                                                       and the person must be on the elevator's dllist.  The person's
                                                       ptr will be used to delete the person from the elevator's dllist. */

extern void *person(void *arg);                  /* The person thread */

/* ------------------------------------------------------------------ */
/* Procedures that you must define: */

extern void initialize_simulation(Elevator_Simulation *es);  /* Called once at the beginning of the simulation */
extern void initialize_elevator(Elevator *e);                /* Called once for each elevator, before the thread is created */
extern void initialize_person(Person *p);                    /* Called once for each person, before the thread is created */

/* The following is called when a person first enters the system.  
   It should block until an elevator is on the person's "from" floor
   and the door is open.  When it does so, the person's e field
   should point to the elevator on which the person should get.  */

extern void wait_for_elevator(Person *p);


/* The following is called after a person gets on the elevator.  It should block
   until the person is at his/her destination floor, and the elevator's
   door is open. */

extern void wait_to_get_off_elevator(Person *p);


/* The following is called after a person gets off the elevator. */

extern void person_done(Person *p);


/* The following is the main elevator thread.  It can only move and open/close
   its doors using the given procedures. */

extern void *elevator(void *arg);   /* The arg is the thread's Elevator struct */
