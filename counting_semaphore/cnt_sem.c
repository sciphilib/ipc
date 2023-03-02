#include <fcntl.h>      // O_CREAT
#include <semaphore.h>  // set_t
#include <stdio.h>      // printf, perror

#define SEM_NAME "/named_semaphore"

int main (int argc, char **argv) {
  sem_t * sem;
  
  if (argc == 2) {
    printf("Dropping semaphore...\n");

    // oflag == 0 then using an existing semaphore
    if ( (sem = sem_open(SEM_NAME, 0)) == SEM_FAILED ) {
      perror("sem_open");
      return 1;
    }

    // increment (unlock) the semaphore
    sem_post(sem);

    perror("sem_post");
    printf("Semaphore dropped.\n");
    return 0;
  }

  // creare a new semaphore
  if ( (sem = sem_open(SEM_NAME, O_CREAT, 0777, 0)) == SEM_FAILED ) {
    perror("sem_open");
    return 1;
  }

  printf("Semaphore is taken.\nWaiting for it to be dropped.\n");
  
  // decrement (lock) the semaphore 
  if (sem_wait(sem) < 0){
    perror("sem_wait");
  }

  // close the semaphore (allow any resources has allocated)
  if (sem_close(sem) < 0) {
    perror("sem_close");
  }

  return 0;
}
