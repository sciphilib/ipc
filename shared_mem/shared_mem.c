#include <sys/mman.h>   // sh_open, mmap
#include <sys/types.h>  // sh_open
#include <stdio.h>      // perror
#include <fcntl.h>      // io syscalls: open, O_CREAT
#include <unistd.h>     // read, close, mmap
#include <string.h>     // strcmp

#define SHM_MEM_NAME "shm_object"
#define SHM_MEM_SIZE 50
#define SHM_CREATE 1
#define SHM_PRINT  3
#define SHM_CLOSE  4

void usage(const char *s) {
  printf("Usage: %s <create|write|read|unlink> ['text']\n", s);
}

int main (int argc, char ** argv) {
  int shm, len, cmd, mode = 0;
  char *addr;

  if (argc < 2) {
    usage(argv[0]);
    return 1;
  }

  if ( (!strcmp(argv[1], "create") || !strcmp(argv[1], "write" )) && (argc == 3) ) {
    len = strlen(argv[2]);
    len = (len <= SHM_MEM_SIZE) ? len : SHM_MEM_SIZE;
    mode = O_CREAT;
    cmd = SHM_CREATE;
  } else if (!strcmp(argv[1], "read")) {
    cmd = SHM_PRINT;
  } else if (!strcmp(argv[1], "unlink")) {
    cmd = SHM_CLOSE;
  } else {
    usage(argv[0]);
    return 1;
  }

  // S_IRWXU read, write, execute owner
  // S_IRWXG read, write, execute group
  // S_IRWXO read, write, execute others
  // equals to 0777
  if ( (shm = shm_open(SHM_MEM_NAME, mode | O_RDWR, (S_IRWXU|S_IRWXG|S_IRWXO))) == -1 ) {
    perror("shm_open");
    return 1;
  }

  if (cmd == SHM_CREATE) {
    if ( ftruncate(shm, SHM_MEM_SIZE+1) == -1) {
      perror("ftruncate");
      return 1;
    }
  }

  // map files into memory
  addr = mmap(0, SHM_MEM_SIZE+1, PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
  if (addr == MAP_FAILED) {
    perror("mmap");
    return 1;
  }

  switch(cmd) {
  case SHM_CREATE:
    memcpy(addr, argv[2], len);
    addr[len] = '\0';
    printf("Shared memory was filled in. You may run '%s print' to see value.\n", argv[0]);
    break;
  case SHM_PRINT:
    printf("Got from shared memory: %s\n", addr);
    break;
  }

  munmap(addr, SHM_MEM_SIZE);
  close(shm);

  if (cmd == SHM_CLOSE) {
    shm_unlink(SHM_MEM_NAME);
  }
  return 0;
}
