/* Uses select and gettimeofday to perform sleeping of sub-second intervals */

void *finesleep_initialize(int cheat);
void finesleep_sleep(void *fs, double time);
double finesleep_time(void *fs);
void finesleep_free(void *a);

