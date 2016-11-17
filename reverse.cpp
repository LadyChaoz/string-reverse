// Anne Warden
// cs375 - project 6
// reverse.cpp

#include <semaphore.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <algorithm> // reverse
#include <mqueue.h>

#include "shared.h"

using namespace std;

int main(int argc, char *argv[])
{
  // Get the semaphore pointer
  sem_t *empty = sem_open(argv[1], 0);
  sem_t *full = sem_open(argv[2], 0);

  // Open shared memory
  int mfd = shm_open(argv[3], O_RDWR | O_CREAT, 0600);
  void *shared_memory = mmap(
     0, sizeof(struct shared_use_st),
     PROT_READ | PROT_WRITE, MAP_SHARED,
     mfd, 0
  );
  struct shared_use_st *shared_stuff = 
    (struct shared_use_st *)shared_memory;

  // Open Message Queue
  mqd_t m2rid = mq_open(argv[4], O_RDWR);
  char received[TEXT_SZ];
  string send, quit_str = "^D";

  while(true){
    sem_wait(empty);

    // Grab Data from MQueue
    mq_receive(m2rid, received, TEXT_SZ, 0);
    send = received;

    // Process and Send Data via Shared Mem
    reverse(send.begin(), send.end());
    strncpy(shared_stuff->text, send.c_str(), TEXT_SZ);
    shared_stuff->waiting_data = true;
    
    // Allow Other Programs to access
    sem_post(full);
    
    if(!send.compare(quit_str))
      break;
  }
  
  munmap(shared_memory, sizeof(struct shared_use_st));
}
