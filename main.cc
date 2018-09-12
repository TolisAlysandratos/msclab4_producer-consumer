/******************************************************************
 * The Main program 
 ******************************************************************/

#include "helper.h"

global_str global;

void *producer (void *parameter);
void *consumer (void *parameter);

int main (int argc, char **argv)
{
  if (argc != 5)
    {
      cerr << "Error: Incorrect number of command line arguments. " << endl
	   << "Input format: ./main (size of buffer) (number of jobs for each producer) "
	   << "(number of producers) (number of consumers)" << endl;
      return ERROR_INCORRECT_COMMAND_ARGS;
    }
  for (int i = 1; i < argc; i++)
    {
      if (check_arg(argv[1]) < 1)
	{
	  cerr << "Error: Buffer size needs to be a positive integer." << endl;
	  return ERROR_BUFFER_SIZE;
	}
      if (check_arg(argv[2]) < 1)
	{
	  cerr << "Error: Number of jobs need to be a positive integer." << endl;
	  return ERROR_NUM_OF_JOBS;
	}
      if (check_arg(argv[3]) < 1)
	{
	  cerr << "Error: Number of producers need to be a positive integer." << endl;
	  return ERROR_NUM_OF_PROD;
	}
      if (check_arg(argv[4]) < 1)
	{
	  cerr << "Error: Number of consumers need to be a positive integer." << endl;
	  return ERROR_NUM_OF_CONS;
	}
    }
  global.key = ftok("home/os_cw/working/test_key", 'A');
  global.semid = sem_create(global.key, 3);
  if (global.semid == -1)
    {
      cerr << "Error: sem_create returned -1"<< endl;
      return ERROR_SEM_CREATE;
    }
  global.buff_size = check_arg(argv[1]);
  global.prod_jobs = check_arg(argv[2]);
  global.num_prod = check_arg(argv[3]);
  global.num_cons = check_arg(argv[4]);
  global.producerid = new pthread_t[global.num_prod];
  global.consumerid = new pthread_t[global.num_cons];
  global.producerid_num = new int[global.num_prod];
  global.consumerid_num = new int[global.num_cons];
  global.buffer = new int[global.buff_size];
  for (int i = 0; i < global.num_prod; i++)
    global.buffer[i] = 0;
  global.job_in = 0;
  global.job_out = 0;
  sem_init(global.semid, MUTEX, 1);
  sem_init(global.semid, SPACE, global.buff_size);
  sem_init(global.semid, ITEM, 0);
  for (int i=0; i < global.num_prod; i++)
    {
      global.producerid_num[i] = i + 1;
      pthread_create(&global.producerid[i], NULL, producer, &global.producerid_num[i]);
    }
  for (int i=0; i < global.num_cons; i++)
    {
      global.consumerid_num[i] = i + 1;
      pthread_create(&global.consumerid[i], NULL, consumer, &global.consumerid_num[i]);
    }
  for (int i=0; i < global.num_prod; i++)
    pthread_join(global.producerid[i], NULL);  
  for (int i=0; i < global.num_cons; i++)
    pthread_join(global.consumerid[i], NULL);
  sem_close(global.semid);
  delete [] global.producerid;
  delete [] global.consumerid;
  delete [] global.producerid_num;
  delete [] global.consumerid_num;
  delete [] global.buffer;
  return 0;
}

void *producer(void *prod_id_n)
{
  int prod_id_num = *(int*) prod_id_n;
  int job_id;
  int job_dur;
  int i = 0;
  while (i < global.prod_jobs)
  {
    stringstream ss_timeout;
    sleep(rand() % 5 + 1);
    job_dur = rand() % 10 + 1;
    int s;
    struct timespec tspec;
    if (clock_gettime(CLOCK_REALTIME, &tspec) == -1)
      {
	cerr << "Error: clock_gettime() returned -1 on thread Producer("
	     << prod_id_num << ") " << endl;
	pthread_exit(0);
      }
    tspec.tv_sec = TIMEOUT;
    while ((s = sem_timedwait(global.semid, SPACE, &tspec)) == -1 && errno == EINTR)
      {
	continue;
      }
    if (s == -1)
      {
	if (errno == EAGAIN)
	  {
	    break;
	  }
	else
	  {
	    cerr << "Producer(" << prod_id_num << "): Exited with errno "
		 << errno << endl;
	    pthread_exit(0);
	  }
      }
    sem_wait(global.semid, MUTEX);
    if (global.job_in == global.buff_size)
      job_id = 0;
    else
      job_id = global.job_in;
    insertJob(global.buffer, job_dur);
    sem_signal(global.semid, MUTEX);
    sem_signal(global.semid, ITEM);
    stringstream ss;
    ss << "Producer(" << prod_id_num << "): Job id " << job_id
       << " duration " << job_dur << endl;
    cout << ss.str();
    i++;
  }
  stringstream ss_nojobs;
  ss_nojobs << "Producer(" << prod_id_num
	    << "): No more jobs to generate." << endl;
  cout << ss_nojobs.str();
  pthread_exit(0);
}

void *consumer (void *cons_id_n)
{
  int cons_id_num = *(int*) cons_id_n;
  int job_id;
  int job_dur;
  while (true)
  {
    int s;
    struct timespec tspec;
    if (clock_gettime(CLOCK_REALTIME, &tspec) == -1)
      {
	cerr << "Error: clock_gettime() returned -1 on thread Consumer("
	     << cons_id_num << ") " << endl;
	pthread_exit(0);
      }
    tspec.tv_sec = TIMEOUT;    
    while ((s = sem_timedwait(global.semid, ITEM, &tspec)) == -1 && errno == EINTR)
      {
	continue;
      }
    if (s == -1)
      {
	if (errno == EAGAIN)
	  {
	    break;
	  }
	else
	  {
	    cerr << "Consumer(" << cons_id_num << "): Exited with errno "
		 << errno << endl;
	    pthread_exit(0);
	  }
      }
    sem_wait(global.semid, MUTEX);
    if (global.job_out == global.buff_size)
      job_id = 0;
    else
      job_id = global.job_out;
    job_dur = takeJob(global.buffer);
    sem_signal(global.semid, MUTEX);
    sem_signal(global.semid, SPACE);
    stringstream ss;
    ss << "Consumer(" << cons_id_num << "): Job id " << job_id
       << " executing sleep duration " << job_dur << endl;
    cout << ss.str();
    sleep(job_dur);
    stringstream ssc;
    ssc << "Consumer(" << cons_id_num << "): Job id " << job_id
	<< " completed" << endl;
    cout << ssc.str();
  }
  stringstream ss_nojobs;
  ss_nojobs << "Consumer(" << cons_id_num << "): No more jobs left." << endl;
  cout << ss_nojobs.str();
  pthread_exit(0);
}
