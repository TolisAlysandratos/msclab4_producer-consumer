/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_timedwait - Same as sem_wait but with a timeout option
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 * insertJob - Inserts a job of input value in the buffer
 * takeJob - Take a job from the buffer, returns its input value and
             sets its buffer value to zero
 ******************************************************************/

# include "helper.h"

int check_arg (char *buffer)
{
  int i, num = 0, temp = 0;
  if (strlen (buffer) == 0)
    return -1;
  for (i=0; i < (int) strlen (buffer); i++)
  {
    temp = 0 + buffer[i];
    if (temp > 57 || temp < 48)
      return -1;
    num += pow (10, strlen (buffer)-i-1) * (buffer[i] - 48);
  }
  return num;
}

int sem_create (key_t key, int num)
{
  int id;
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0)
    return -1;
  return id;
}

int sem_init (int id, int num, int value)
{
  union semun semctl_arg;
  semctl_arg.val = value;
  if (semctl (id, num, SETVAL, semctl_arg) < 0)
    return -1;
  return 0;
}

void sem_wait(int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  semop (id, op, 1);
}

int sem_timedwait (int id, short unsigned int num, struct timespec *timeout)
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  int s;
  s = semtimedop(id, op, 1, timeout);
  return s;
}

void sem_signal (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 1, SEM_UNDO}
  };
  semop (id, op, 1);
}

int sem_close (int id)
{
  if (semctl (id, 0, IPC_RMID, 0) < 0)
    return -1;
  return 0;
}

void insertJob(int* buffer, int job_val)
{
  if (global.job_in == global.buff_size)
    global.job_in = 0;
  while (buffer[global.job_in] != 0)
    {
      global.job_in++;
      if (global.job_in == global.buff_size)
	global.job_in = 0;
    }
  buffer[global.job_in] = job_val;
  global.job_in++;
}

int takeJob(int* buffer)
{
  if (global.job_out == global.buff_size)
    global.job_out = 0;
  while (buffer[global.job_out] == 0)
    {
      global.job_out++;
      if (global.job_out == global.buff_size)
	global.job_out = 0;
    }
  int consume;
  consume = buffer[global.job_out];
  buffer[global.job_out] = 0;
  global.job_out++;
  return consume;
}
