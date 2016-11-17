// Anne Warden
// cs375 - Project 6
// master.cpp

#include <sys/wait.h>
#include <stdlib.h>
#include <semaphore.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <mqueue.h>

#include "shared.h"

using namespace std;

void quit(int sig)
{
  struct sigaction def_act;
  def_act.sa_handler = SIG_DFL;
  sigemptyset(&(def_act.sa_mask));
  def_act.sa_flags = 0;
  sigaction(sig, &def_act, 0);

  //kill(fpid1, SIGTERM);
  //kill(fpid2, SIGTERM);

  close(shm_open("/cs375_MSEG", O_RDWR | O_CREAT, 0600));
  
  mq_unlink("/cs375_MQ");
  shm_unlink("/cs375_MSEG");
  sem_unlink("/cs375_sem1");
  sem_unlink("/cs375_sem2");
  kill(getpid(), SIGTERM);

  return;
}

int main ()
{
  // Declare semaphores
  string sem_name1 = "/cs375_sem1", sem_name2 = "/cs375_sem2";
  sem_t *empty = sem_open(sem_name1.c_str(), O_CREAT | O_EXCL, 0600, 1),
    *full = sem_open(sem_name2.c_str(), O_CREAT | O_EXCL, 0600, 0);

  // Set up shared memory
  string shm_name = "/cs375_MSEG";
  const char *memory = shm_name.c_str();
  int mfd = shm_open("cs375_MSEG", O_RDWR | O_CREAT, 0600);
  ftruncate(mfd, sizeof(struct shared_use_st));
  void *shared_memory = mmap(
     0, sizeof(struct shared_use_st),
     PROT_READ | PROT_WRITE, MAP_SHARED,
     mfd, 0
  );
  struct shared_use_st *shared_stuff = 
    (struct shared_use_st *)shared_memory;
  shared_stuff->waiting_data = false;
   
  // Set up Message Queue
  string mQ_name = "/cs375_MQ";
  const char *m2r = mQ_name.c_str();

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = TEXT_SZ;
  attr.mq_curmsgs = 0;

  mqd_t m2rid = mq_open(m2r, O_RDWR | O_CREAT, 0600, &attr);

  // Set up signal handler for ^D
  struct sigaction endact, oldact;
  endact.sa_handler = quit;
  sigemptyset(&(endact.sa_mask));
  endact.sa_flags = 0;
  sigaction(SIGINT, &endact, &oldact);

  // Set up signal for SIGUSR1
  sigset_t waitset;
  int sig;
  
  sigemptyset(&waitset);
  sigaddset(&waitset, SIGUSR1);
  sigprocmask(SIG_BLOCK, &waitset, NULL);

  // Fork to reverse
  int fpid1 = fork();
  if (fpid1 == 0) {
    execl("reverse", "reverse", sem_name1.c_str(), sem_name2.c_str(),
	  memory, m2r, (char *)0);
    cerr << "reverse exec failed, exiting" << endl;
    exit(1);
  }

  // Fork to upper
  int fpid2 = fork();
  if(fpid2 == 0){
    execl("upper", "upper", sem_name1.c_str(), sem_name2.c_str(), 
	  memory, (char *)0);
    cerr << "reverse exec failed, exiting" << endl;
    exit(1);
  }

  /* Unmap from shared memory once it has been passed to the child processes
   * Safe because master does not use shared mem.
   */
  munmap(shared_memory, sizeof(struct shared_use_st));

  // Run Master
  string message, quit_str = "^D";
  cout << "Start" << endl;
  while(true)
    {
      // Do the thing that needs doing
      cout << "> ";
      getline(cin, message);
      if(!message.compare(quit_str))
	break;
      // Pass it to the Message Queue
      mq_send(m2rid, message.c_str(), TEXT_SZ, 1);
      sigwait(&waitset, &sig);
    }

  mq_send(m2rid, "^D", TEXT_SZ, 1);
  waitpid(fpid2, (int *)0, WEXITED);
  //kill(fpid1, SIGTERM);
  //kill(fpid2, SIGTERM);

  close(shm_open(memory, O_RDWR | O_CREAT, 0600));

  mq_unlink("/cs375_MQ");
  shm_unlink("/cs375_MSEG");
  sem_unlink("/cs375_sem1");
  sem_unlink("/cs375_sem2");
}
