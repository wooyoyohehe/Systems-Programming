/* elevator_skeleton.c
   Header file for the elevator threads lab.
   Jim Plank
   CS560
   Lab 2
   January, 2009
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "names.h"
#include "elevator.h"
#include "finesleep.h"
#include "dllist.h"

#define talloc(ty, sz) (ty *) malloc ((sz) * sizeof(ty))

void *FINESLEEPER;

void move_to_floor(Elevator *e, int floor)
{
  double diff;
  pthread_mutex_lock(e->lock);
  if (e->door_open) {
    fprintf(stderr, "Error at time %.3lf: Move to floor on elevator %02d with the door open.\n",
            finesleep_time(FINESLEEPER), e->id);
    exit(1);
  }
  if (e->moving) {
    fprintf(stderr, "Error at time %.3lf: Move to floor on elevator %02d that is already moving.\n",
            finesleep_time(FINESLEEPER), e->id);
    exit(1);
  }
  diff = floor - e->onfloor;
  if (diff < 0) diff = -diff;
  diff *= e->es->floor_to_floor_time;
  e->moving = 1;
  pthread_mutex_lock(e->es->lock);
  printf("%8.3lf: Elevator %02d moving from floor %02d to floor %02d.\n", 
       finesleep_time(FINESLEEPER), e->id, e->onfloor, floor);
  fflush(stdout);
  pthread_mutex_unlock(e->es->lock);
  pthread_mutex_unlock(e->lock);
  finesleep_sleep(FINESLEEPER, diff);
  pthread_mutex_lock(e->lock);
  pthread_mutex_lock(e->es->lock);
  printf("%8.3lf: Elevator %02d arrives at floor %02d.\n", 
       finesleep_time(FINESLEEPER), e->id, floor);
  fflush(stdout);
  pthread_mutex_unlock(e->es->lock);
  e->moving = 0;
  e->onfloor = floor;
  pthread_mutex_unlock(e->lock);
}

void open_door(Elevator *e)
{
  if (e->door_open) {
    fprintf(stderr, "Error at time %.3lf: Open door called on elevator %02d with the door already open.\n",
            finesleep_time(FINESLEEPER), e->id);
    exit(1);
  }
  if (e->moving) {
    fprintf(stderr, "Error at time %.3lf: Open door called on elevator %02d with the elevator moving.\n",
            finesleep_time(FINESLEEPER), e->id);
    exit(1);
  }
  pthread_mutex_lock(e->es->lock);
  printf("%8.3lf: Elevator %02d opening its door.\n", finesleep_time(FINESLEEPER), e->id);
  fflush(stdout);
  pthread_mutex_unlock(e->es->lock);
  finesleep_sleep(FINESLEEPER, e->es->door_time);
  pthread_mutex_lock(e->lock);
  pthread_mutex_lock(e->es->lock);
  printf("%8.3lf: Elevator %02d door is open.\n", finesleep_time(FINESLEEPER), e->id);
  fflush(stdout);
  pthread_mutex_unlock(e->es->lock);
  e->door_open = 1;
  pthread_mutex_unlock(e->lock);
}

void close_door(Elevator *e)
{
  if (!e->door_open) {
    fprintf(stderr, "Error at time %.3lf: Close door called on elevator %02d with the door already open.\n",
            finesleep_time(FINESLEEPER), e->id);
    exit(1);
  }
  if (e->moving) {
    fprintf(stderr, "Error at time %.3lf: Close door called on elevator %02d with the elevator moving.\n",
            finesleep_time(FINESLEEPER), e->id);
    exit(1);
  }
  pthread_mutex_lock(e->es->lock);
  printf("%8.3lf: Elevator %02d closing its door.\n", finesleep_time(FINESLEEPER), e->id);
  fflush(stdout);
  pthread_mutex_unlock(e->es->lock);
  finesleep_sleep(FINESLEEPER, e->es->door_time);
  pthread_mutex_lock(e->lock);
  pthread_mutex_lock(e->es->lock);
  printf("%8.3lf: Elevator %02d door is closed.\n", finesleep_time(FINESLEEPER), e->id);
  fflush(stdout);
  pthread_mutex_unlock(e->es->lock);
  e->door_open = 0;
  pthread_mutex_unlock(e->lock);
}

void get_on_elevator(Person *p)
{
  Elevator *e;

  e = p->e;
  if (e == NULL) {
    fprintf(stderr, "Error at time %.3lf: %s %s called get_on_elevator with a NULL elevator.\n",
            finesleep_time(FINESLEEPER), p->fname, p->lname);
    exit(1);
  }
    
  pthread_mutex_lock(e->lock);
  if (!e->door_open) {
    fprintf(stderr, "Error at time %.3lf: %s %s get_on_elevator(%02d) - door closed.\n",
            finesleep_time(FINESLEEPER), p->fname, p->lname, e->id);
    exit(1);
  }
  if (e->moving) {
    fprintf(stderr, "Error at time %.3lf: %s %s get_on_elevator(%02d) - elevator moving.\n",
            finesleep_time(FINESLEEPER), p->fname, p->lname, e->id);
    exit(1);
  }
  if (e->onfloor != p->from) {
    fprintf(stderr, "Error at time %.3lf: %s %s get_on_elevator(%02d) - Elevator on wrong floor.\n",
            finesleep_time(FINESLEEPER), p->fname, p->lname, e->id);
    exit(1);
  }
  dll_append(e->people, new_jval_v((void *) p));
  p->ptr = e->people->blink;
  pthread_mutex_lock(e->es->lock);
  printf("%8.3lf: %s %s gets on elevator %02d on floor %02d.\n", finesleep_time(FINESLEEPER), 
          p->fname, p->lname, e->id, e->onfloor);
  fflush(stdout);
  pthread_mutex_unlock(e->es->lock);
  pthread_mutex_unlock(e->lock);
}

void get_off_elevator(Person *p)
{
  Elevator *e;

  e = p->e;
  if (e == NULL) {
    fprintf(stderr, "Error at time %.3lf: %s %s called get_off_elevator with a NULL elevator.\n",
            finesleep_time(FINESLEEPER), p->fname, p->lname);
    exit(1);
  }
    
  pthread_mutex_lock(e->lock);
  if (!e->door_open) {
    fprintf(stderr, "Error at time %.3lf: %s %s get_off_elevator(%02d) - door closed.\n",
            finesleep_time(FINESLEEPER), p->fname, p->lname, e->id);
    exit(1);
  }
  if (e->moving) {
    fprintf(stderr, "Error at time %.3lf: %s %s get_off_elevator(%02d) - elevator moving.\n",
            finesleep_time(FINESLEEPER), p->fname, p->lname, e->id);
    exit(1);
  }
  if (e->onfloor != p->to) {
    fprintf(stderr, "Error at time %.3lf: %s %s get_off_elevator(%02d) - Elevator on wrong floor.\n",
            finesleep_time(FINESLEEPER), p->fname, p->lname, e->id);
    exit(1);
  }
  dll_delete_node(p->ptr);
  p->ptr = NULL;
  pthread_mutex_lock(p->es->lock);
  printf("%8.3lf: %s %s gets off elevator %02d on floor %02d.\n", finesleep_time(FINESLEEPER), 
          p->fname, p->lname, e->id, e->onfloor);
  fflush(stdout);
  pthread_mutex_unlock(p->es->lock);
  pthread_mutex_unlock(e->lock);
}

void *person(void *arg)
{
  Person *p;

  p = (Person *) arg;
    //printf("person: %s, before lock\n", p->fname);
  pthread_mutex_lock(p->es->lock);
  p->es->npeople_started++;
  printf("%8.3lf: %s %s arrives at floor %02d wanting to go to floor %02d.\n", finesleep_time(FINESLEEPER), 
         p->fname, p->lname, p->from, p->to);
  fflush(stdout);
  pthread_mutex_unlock(p->es->lock);
          
  wait_for_elevator(p);
  get_on_elevator(p);
  wait_to_get_off_elevator(p);
  get_off_elevator(p);
  person_done(p);

  pthread_mutex_lock(p->es->lock);
  printf("%8.3lf: %s %s is done.\n", finesleep_time(FINESLEEPER), p->fname, p->lname);
  fflush(stdout);
  p->es->npeople_finished++;
  pthread_mutex_unlock(p->es->lock);

  pthread_mutex_destroy(p->lock);
  free(p->lock);
  pthread_cond_destroy(p->cond);
  free(p->cond);
  free(p);
  return NULL;
}

void *person_gen(void *arg)
{
  Elevator_Simulation *es;
  pthread_t tid;
  double tosleep;
  Person *p;
  double perc;
  char *s;
  int pn;

  pn = 0;
  es = (Elevator_Simulation *) arg;
  while (1) {
    tosleep = -1.0 * log(1.0 - drand48()) * es->interarrival_time;
    finesleep_sleep(FINESLEEPER, tosleep);
    p = talloc(Person, 1);
    p->fname = FNAMES[lrand48()%200];   
    s = LNAMES[lrand48()%200];   
    p->lname = talloc(char, strlen(s)+20);
    sprintf(p->lname, "%s(%d)", s, pn);
    pn++;

    perc = drand48();
    if (perc < 0.333333) {
      p->from = 1;
      p->to = lrand48()%(es->nfloors-1) + 2;
    } else if (perc < 0.6666667) {
      p->from = lrand48()%(es->nfloors-1) + 2;
      p->to = 1;
    } else {
      p->from = lrand48()%(es->nfloors) + 1;
      do {
        p->to = lrand48()%(es->nfloors) + 1;
      } while (p->from == p->to);
    }

    p->arrival_time = finesleep_time(FINESLEEPER);
    p->e = NULL;
    p->ptr = NULL;
    p->lock = talloc(pthread_mutex_t, 1);
    pthread_mutex_init(p->lock, NULL);
    p->cond = talloc(pthread_cond_t, 1);
    pthread_cond_init(p->cond, NULL);
    p->es = es;
    initialize_person(p);
    if (pthread_create(&tid, NULL, person, (void *) p) != 0) {
      pthread_mutex_lock(es->lock);
      printf("Pthread create for person %d failed\n", pn);
      fflush(stdout);
      exit(1);
    }
    pthread_detach(tid);
  }
}

void usage(char *s)
{
  fprintf(stderr, "usage: elevator nfloors nelevators interarrival opentime floor_to_floor duration seed\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

int main(int argc, char **argv)
{
  Elevator_Simulation ES, *es;
  pthread_t tid;
  int i;
  Elevator *e;
  double duration;
  
  long seed;
  es = &ES;

  if (argc != 8) usage(NULL);

  if (sscanf(argv[1], "%d", &es->nfloors) != 1 || es->nfloors <= 1) {
    usage("Bad nfloors (must be > 1)");
  }
  if (sscanf(argv[2], "%d", &es->nelevators) != 1 || es->nelevators <= 0) {
    usage("Bad nelevators (must be > 0)");
  }
  if (sscanf(argv[3], "%lf", &es->interarrival_time) != 1 || es->interarrival_time <= 0) {
    usage("Bad interarrival (must be > 0)");
  }
  if (sscanf(argv[4], "%lf", &es->door_time) != 1 || es->door_time <= 0) {
    usage("Bad opentime (must be > 0)");
  }
  if (sscanf(argv[5], "%lf", &es->floor_to_floor_time) != 1 || es->floor_to_floor_time <= 0) {
    usage("Bad floor_to_floor (must be > 0)");
  }
  if (sscanf(argv[6], "%lf", &duration) != 1 || duration <= 0) {
    usage("Bad duration (must be > 0)");
  }
  if (sscanf(argv[7], "%ld", &seed) != 1) {
    usage("Bad seed");
    if (seed == -1) seed = time(0);
  }
  srand48(seed);
  es->lock = talloc(pthread_mutex_t, 1);
  pthread_mutex_init(es->lock, NULL);
  es->npeople_started = 0;
  es->npeople_finished = 0;

  FINESLEEPER = finesleep_initialize(1);
  initialize_simulation(es);

  for (i = 0; i < es->nelevators; i++) {
    e = talloc(Elevator, 1);
    e->id = i+1;
    e->onfloor = 1;
    e->door_open = 0;
    e->moving = 0;
    e->people = new_dllist();
    e->lock = talloc(pthread_mutex_t, 1);
    pthread_mutex_init(e->lock, NULL);
    e->cond = talloc(pthread_cond_t, 1);
    pthread_cond_init(e->cond, NULL);
    e->es = es;
    initialize_elevator(e);
    if (pthread_create(&tid, NULL, elevator, (void *) e) != 0) {
      printf("Pthread create for elevator %d failed\n", i);
      exit(1);
    }
    pthread_detach(tid);
  }

  if (pthread_create(&tid, NULL, person_gen, (void *) es) != 0) {
    printf("Pthread create for person_gen failed\n");
    exit(1);
  }
  pthread_detach(tid);

  finesleep_sleep(FINESLEEPER, duration);
  pthread_mutex_lock(es->lock);
  printf("%8.3lf: Simulation Over. %10d Started.  %10d Finished.\n", finesleep_time(FINESLEEPER),
          es->npeople_started, es->npeople_finished);
  exit(0);
  pthread_mutex_unlock(es->lock);
}
