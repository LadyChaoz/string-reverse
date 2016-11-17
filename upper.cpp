// Anne Warden
// cs375 - Project 6
// upper.cpp

#include <sys/wait.h>
#include <stdlib.h>
#include <semaphore.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

#include "shared.h"

using namespace std;

int main(int argc, char *argv[])
{
  // Open the semaphore
  sem_t *empty = sem_open(argv[1], 0);
  sem_t *full = sem_open(argv[2], 0);

  // Open the shared memory
  int mfd = shm_open(argv[3], O_RDWR | O_CREAT, 0600);
  void *shared_memory = mmap(
     0, sizeof(struct shared_use_st),
     PROT_READ | PROT_WRITE, MAP_SHARED,
     mfd, 0
  );
  struct shared_use_st *shared_stuff =
    (struct shared_use_st *)shared_memory;
 
  
  //struct shared_use_st *shared_stuff = (struct shared_use_st *)argv[3];
  string message, quit_str = "^D";

  while(true) {
    sem_wait(full);
    if(shared_stuff->waiting_data) {
      message = shared_stuff->text;
      if(!message.compare(quit_str)){
	sem_post(empty);
	munmap(shared_memory, sizeof(struct shared_use_st));
	return 0;
      }
      shared_stuff->waiting_data = false;
      char str[TEXT_SZ];
      strncpy(str, message.c_str(), TEXT_SZ);
      for(int i = 0; i < message.length(); i++)
	str[i] = toupper(str[i]);
      cout << str << endl;
    }
    sem_post(empty);
    sigval dummy;
    sigqueue(getppid(), SIGUSR1, dummy);
  }
  
  //sem_post(empty);
  //munmap(shared_memory, sizeof(struct shared_use_st));
}
