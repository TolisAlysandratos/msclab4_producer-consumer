/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures,
 * the semaphore values and error codes.
 ******************************************************************/
# ifndef HELPER_H
# define HELPER_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>
# include <sstream>
# include <time.h>
using namespace std;

# define MUTEX 0
# define ITEM 1
# define SPACE 2
# define TIMEOUT 20 // unit: seconds

# define ERROR_INCORRECT_COMMAND_ARGS -1;
# define ERROR_BUFFER_SIZE -2;
# define ERROR_NUM_OF_JOBS -3;
# define ERROR_NUM_OF_PROD -4;
# define ERROR_NUM_OF_CONS -5;
# define ERROR_SEM_CREATE -6;

struct global_str
{
  pthread_t* producerid;
  int* producerid_num;
  pthread_t* consumerid;
  int* consumerid_num;
  int prod_jobs;
  int* buffer;  
  int num_prod;
  int num_cons;
  int buff_size;
  int job_in;
  int job_out;
  key_t key;
  int semid;
};

extern global_str global;

union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

int check_arg (char *);
int sem_create (key_t, int);
int sem_init (int, int, int);
void sem_wait(int id, short unsigned int num);
int sem_timedwait (int, short unsigned int, struct timespec *timeout);
void sem_signal (int, short unsigned int);
int sem_close (int);
void insertJob(int* buffer, int job_val);
int takeJob(int* buffer);

# endif
