#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "jrb.h"
#define talloc(ty, sz) (ty *) malloc ((sz) * sizeof(ty))

typedef struct {
  int fd[2];
  int cheat;
  double stime;
  JRB tree;
  pthread_mutex_t *lock;
} Finesleep;

void *finesleep_initialize(int cheat)
{
  Finesleep *fs;
  struct timeval tv;
  struct timezone tz;

  fs = talloc(Finesleep, 1);
  fs->cheat = cheat;
  fs->tree = make_jrb();
  fs->lock = talloc(pthread_mutex_t, 1);
  pthread_mutex_init(fs->lock, NULL);
    if( pipe(fs->fd) == -1 ) {
        perror("finesleep_initialize: pipe()");
        exit(1);
    };
  if (cheat) {
    fs->stime = 0;
  } else {
    gettimeofday(&tv, &tz);
    fs->stime = tv.tv_usec;
    fs->stime /= 1000000.0;
    fs->stime += tv.tv_sec;
  }
  return (void *) fs;
}
  
void finesleep_sleep(void *a, double t)
{
  Finesleep *fs;
  fd_set r;
  struct timeval tv;
  double newtime=0;
  JRB ptr=NULL;
  double diff;

  fs = (Finesleep *) a;
  if (fs->cheat) {
    newtime = fs->stime + t;
    pthread_mutex_lock(fs->lock);
    ptr = jrb_insert_dbl(fs->tree, newtime, new_jval_i(0));
    diff = newtime - fs->stime;
    pthread_mutex_unlock(fs->lock);
  } else {
    diff = t;
  }
  while (1) {
    tv.tv_sec = diff;
    diff -= tv.tv_sec;
    diff *= 1000000.0;
    tv.tv_usec = diff;
    FD_ZERO(&r);
    FD_SET(fs->fd[0], &r);
    select(fs->fd[0]+1, &r, NULL, NULL, &tv);
    if (fs->cheat) {
      pthread_mutex_lock(fs->lock);
      if (fs->tree->flink->key.d == newtime) {
        jrb_delete_node(ptr);
        fs->stime = newtime;
        pthread_mutex_unlock(fs->lock);
        return;
      } else {
        diff = newtime - fs->tree->flink->key.d;
        pthread_mutex_unlock(fs->lock);
      }
    } else {
      return;
    }
  }
}

double finesleep_time(void *a)
{
  Finesleep *fs;
  double now;
  struct timeval tv;
  struct timezone tz;

  fs = (Finesleep *) a;
  if (fs->cheat) return fs->stime;
  gettimeofday(&tv, &tz);
  now = tv.tv_usec;
  now /= 1000000.0;
  now += tv.tv_sec;
  return now - fs->stime;
}

void finesleep_free(void *a)
{
  Finesleep *fs;

  fs = (Finesleep *) a;
  close(fs->fd[0]);
  close(fs->fd[1]);
  jrb_free_tree(fs->tree);
  pthread_mutex_destroy(fs->lock);
  free(fs->lock);
  free(fs);
}
