#include <stdio.h>    // gets
#include <pthread.h>  // ptread_mutex
#include <unistd.h>   // usleep
#include <errno.h>    // EBUSY
#include <stdlib.h>   // atoi

static int counter; // shared resource
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void incr_counter(void *p) {
  do {
    usleep(10); // time between mutex locks 10 microseconds
    pthread_mutex_lock(&mutex);
    counter++;
    printf("%d\n", counter);
    sleep(1); // 1 second
    pthread_mutex_unlock(&mutex);
  } while (1);
}

void reset_counter(void *p) {
  char buf[10];
  int num = 0;
  int rc;

  pthread_mutex_lock(&mutex);
  printf("Enter the number and press 'Enter' to initialize the counter with new value anytime.\n");
  sleep(3);
  pthread_mutex_unlock(&mutex);

  do {
    if (fgets(buf, 10, stdin) != buf) return;
    num = atoi(buf);
    if ( (rc = pthread_mutex_trylock(&mutex)) == EBUSY ) {
      printf("Mutex is already locked by another process.\nTrying to lock...\n");
      pthread_mutex_lock(&mutex);
    } else if (rc == 0) {
      printf("You're on time.\n");
    } else {
      printf("Error: %d\n", rc);
      return;
    }
    counter = num;
    printf("New value for counter is %d\n", counter);
    pthread_mutex_unlock(&mutex);
  } while (1);
}


int main(int argc, char **argv) {
  pthread_t thread1;
  pthread_t thread2;
  counter = 0;

  pthread_create(&thread1, NULL, (void *)&incr_counter, NULL);
  pthread_create(&thread2, NULL, (void *)&reset_counter, NULL);

  //pthread_join(thread2, NULL);
  
  return 0;
}
