#include <sys/stat.h>   // mkfifo
#include <sys/types.h>  // mkfifo
#include <stdio.h>      // perror
#include <fcntl.h>      // io syscalls: open, O_RDONLY
#include <unistd.h>     // read, close
#include <string.h>

//-----------------
// Named pipe. To use a file correctly, it is necessary to open it
// simultaneosly by two processes/threads. One to receive data (read the file)
// and the other to send it (write to the file).
//
// For example. First ps: run the program. Second ps: write to pipe.
//-----------------

#define NAMEDPIPE_NAME "pipe"
#define BUFSIZE 50

extern int errno;

int main (int argc, char ** argv) {
  int file_descriptor, len;
  char buf[BUFSIZE];

  // create pipe
  if ( mkfifo(NAMEDPIPE_NAME, 0777) ) {
    perror("mkfifo");
    return 1;
  }
  printf("%s is created\n", NAMEDPIPE_NAME);

  // open for readonly
  if ( (file_descriptor = open(NAMEDPIPE_NAME, O_RDONLY) ) <= 0 ) {
    perror("open");
    return 1;
  }
  printf("%s is opened\n", NAMEDPIPE_NAME);

  
  do {
    memset(buf, '\0', BUFSIZE);
    // trying to read
    if ( (len = read(file_descriptor, buf, BUFSIZE - 1)) <= 0 ) {
      perror("read");
      close(file_descriptor);
      remove(NAMEDPIPE_NAME);
      return 0;
    }
    printf("Message from named pipe (%d): %s\n", len, buf);
  } while (1);
}
