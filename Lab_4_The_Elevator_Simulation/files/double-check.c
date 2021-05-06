#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fields.h"
#include "jrb.h"

#define talloc(ty, sz) (ty *) malloc ((sz) * sizeof(ty))

typedef struct {
  double time;
  int id;
  int door;
  int floor;
  int state;
} Elevator;

typedef struct {
  char *name;
  int from;
  int to;
  Elevator *e;
  int state;
} Person;
  
Elevator *get_elevator(int id, JRB elevators)
{
  Elevator *e;
  JRB tmp;
 
  tmp = jrb_find_int(elevators, id);
  if (tmp != NULL) return (Elevator *) tmp->val.v;

  e = talloc(Elevator, 1);
  e->time = 0;
  e->id = id;
  e->door = 0;
  e->floor = 1;
  e->state = 'R';
  jrb_insert_int(elevators, id, new_jval_v((void *) e));
  return e;
}

int main(int argc, char **argv)
{
  IS is;
  JRB people, tmp;
  JRB elevators;
  double t;
  Elevator *e;
  char name[100];
  Person *p;

  elevators = make_jrb();
  people = make_jrb();
  is = new_inputstruct(NULL);

  while (get_line(is) > 0) {
    sscanf(is->fields[0], "%lf", &t);
    if (strcmp(is->fields[1], "Elevator") == 0) {
      e = get_elevator(atoi(is->fields[2]), elevators);
      if (strcmp(is->fields[3], "opening") == 0) {
        if (e->door != 0) {
          printf("Line %d: Elevator %d opening a door that's already open\n", is->line, e->id);
          exit(1);
        }
        if (e->state == 'O') {
          printf("Line %d: Elevator %d opening a door twice\n", is->line, e->id);
          exit(1);
        }
        if (e->state == 'C') {
          printf("Line %d: Elevator %d opening a door that is closing\n", is->line, e->id);
          exit(1);
        }
        e->state = 'O';
        e->time = t;
      } else if (strcmp(is->fields[3], "closing") == 0) {
        if (e->door != 1) {
          printf("Line %d: Elevator %d closing a door that's already closed\n", is->line, e->id);
          exit(1);
        }
        if (e->state == 'C') {
          printf("Line %d: Elevator %d closing a door twice\n", is->line, e->id);
          exit(1);
        }
        if (e->state == 'O') {
          printf("Line %d: Elevator %d closing a door that is opening\n", is->line, e->id);
          exit(1);
        }
        e->state = 'C';
        e->time = t;
      } else if (strcmp(is->fields[5], "closed.") == 0) {
        if (e->state != 'C') {
          printf("Line %d: Elevator %d closed a door that was not closing.\n", is->line, e->id);
          exit(1);
        }
        e->state = 'R';
        e->door = 0;
        e->time = t;
      } else if (strcmp(is->fields[5], "open.") == 0) {
        if (e->state != 'O') {
          printf("Line %d: Elevator %d opened a door that was not opening.\n", is->line, e->id);
          exit(1);
        }
        e->state = 'R';
        e->door = 1;
        e->time = t;
      } else if (strcmp(is->fields[3], "moving") == 0) {
        if (e->state != 'R') {
          printf("Line %d: Elevator %d moving from a non-rest state.\n", is->line, e->id);
          exit(1);
        }
        if (e->door != 0) {
          printf("Line %d: Elevator %d moving when the door is open.\n", is->line, e->id);
          exit(1);
        }
        if (atoi(is->fields[6]) != e->floor) {
          printf("Line %d: Elevator %d moving from a bad floor.\n", is->line, e->id);
          exit(1);
        }
        e->floor = atoi(is->fields[9]);
        e->state = 'M';
        e->time = t;
      } else if (strcmp(is->fields[3], "arrives") == 0) {
        if (e->state != 'M') {
          printf("Line %d: Elevator %d arriving from a non-moving state.\n", is->line, e->id);
          exit(1);
        }
        if (e->door != 0) {
          printf("Line %d: Elevator %d arriving when the door is open.\n", is->line, e->id);
          exit(1);
        }
        if (atoi(is->fields[6]) != e->floor) {
          printf("Line %d: Elevator %d arriving at the wrong floor (%d).\n", is->line, e->id, e->floor);
          exit(1);
        }
        e->state = 'R';
        e->time = t;
      }
    } else if (strcmp(is->fields[1], "Simulation") == 0) {
      exit(0);
    } else {
      sprintf(name, "%s %s", is->fields[1], is->fields[2]);
      if (strcmp(is->fields[3], "arrives") == 0) {
        if (jrb_find_str(people, name) != NULL) {
          printf("%d: Duplicate person %s\n", is->line, name);
          exit(1);
        }
        p = talloc(Person, 1);
        p->name = strdup(name);
        jrb_insert_str(people, p->name, new_jval_v((void *) p));
        p->from = atoi(is->fields[6]);
        p->to = atoi(is->fields[12]);
        p->state = 'A';
      } else {
        tmp = jrb_find_str(people, name);
        if (tmp == NULL) {
          printf("Line %d: Person %s doesn't exist\n", is->line, name);
          exit(1);
        }
        p = (Person *) tmp->val.v;
        if (strcmp(is->fields[4], "on") == 0) {
          if (p->state != 'A') {
            printf("Line %d: Person %s not in arriving state when getting on an elevator\n", is->line, p->name);
            exit(1);
          }
          if (atoi(is->fields[9]) != p->from) {
            printf("Line %d: Person %s not getting on the proper floor\n", is->line, p->name);
            exit(1);
          }
          e = get_elevator(atoi(is->fields[6]), elevators);
          if (e->floor != p->from) {
            printf("Line %d: Person %s getting on an elevator not on the right floor\n", is->line, p->name);
            exit(1);
          }
          if (e->door != 1) {
            printf("Line %d: Person %s getting on an elevator whose door isn't open.\n", is->line, p->name);
            exit(1);
          }
          if (e->state != 'R') {
            printf("Line %d: Person %s getting on an elevator who is not at rest.\n", is->line, p->name);
            exit(1);
          }
          p->state = 'O';
          p->e = e;
        } else if (strcmp(is->fields[4], "off") == 0) {
          if (p->state != 'O') {
            printf("Line %d: Person %s not on the elevator when getting off\n", is->line, p->name);
            exit(1);
          }
          if (atoi(is->fields[9]) != p->to) {
            printf("Line %d: Person %s not getting off on the proper floor\n", is->line, p->name);
            exit(1);
          }
          e = get_elevator(atoi(is->fields[6]), elevators);
          if (e != p->e) {
            printf("Line %d: Person %s getting off the wrong elevator\n", is->line, p->name);
            exit(1);
          }
          if (e->floor != p->to) {
            printf("Line %d: Person %s getting off an elevator not on the right floor\n", is->line, p->name);
            exit(1);
          }
          if (e->door != 1) {
            printf("Line %d: Person %s getting off an elevator whose door isn't open.\n", is->line, p->name);
            exit(1);
          }
          if (e->state != 'R') {
            printf("Line %d: Person %s getting off an elevator who is not at rest.\n", is->line, p->name);
            exit(1);
          }
          p->state = 'F';
        } else if (strcmp(is->fields[4], "done.") == 0) {
          if (p->state != 'F') {
            printf("Line %d: Person %s done before getting off the elevator\n", is->line, p->name);
            exit(1);
          }
          jrb_delete_node(tmp);
          free(p->name);          
          free(p);
        }
      }
    }
  }
  exit(0);
}



