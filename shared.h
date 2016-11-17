// Anne Warden
// CS375 - Project 6
// shared.h

#include <sys/mman.h>
#include <fcntl.h>

#define TEXT_SZ 256

struct shared_use_st {
  bool waiting_data;
  char text[TEXT_SZ];
};
