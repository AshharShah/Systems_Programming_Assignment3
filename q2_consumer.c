// structure to store messages in shared memory
#define TEXT_SZ 2048
struct shared_use_st {
    char some_text[TEXT_SZ];
};

// macros that define the semaphore names
#define SEMAPHORE_NAME_EMPTIES "/ourSemaphore1.dat"
#define SEMAPHORE_NAME_FULLS "/ourSemaphore2.dat"

// the required libraries
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <stdio.h>
#include <fcntl.h>
#include <semaphore.h>

int main(){    /* We now make the shared memory accessible to the program. */
    int running = 1;
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    int shmid;

    // initialize the semaphores
    sem_t *semIDE;
    sem_t *semIDF;
    semIDE = sem_open(SEMAPHORE_NAME_EMPTIES, O_CREAT, S_IRUSR|S_IWUSR, 1);
    semIDF = sem_open(SEMAPHORE_NAME_FULLS, O_CREAT, S_IRUSR|S_IWUSR, 0);

    // check for errors in semaphore initialization
    if (semIDE == SEM_FAILED) {
        perror("Can't open semaphore");
        exit(EXIT_FAILURE);
    }
    if (semIDF == SEM_FAILED) {
        perror("Can't open semaphore");
        exit(EXIT_FAILURE);
    }

    srand((unsigned int)getpid());

    // create a shared memory segment
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    // attach the shared memory segment so it is available to the program
    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Memory attached at %p\n", shared_memory);
    shared_stuff = (struct shared_use_st *)shared_memory;

    // run a loop till we do not encounter an end of file
    while(running) {
        // decrement the full/producer semaphore (init value 0)
        sem_wait(semIDF);
        // read the text written by the producer in the shared memory segment
        printf("You wrote: %s", shared_stuff->some_text);
        // check if we encounter run, if so terminate the loop
        if (strncmp(shared_stuff->some_text, "end", 3) == 0) {
            running = 0;
        }
        // increment the empty semaphore (init value 1)
        sem_post(semIDE);
    }

    // close all the shared memory segments and the semaphores
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    if(sem_close(semIDE) != 0){
        perror("Can't close semaphore");
        exit(EXIT_FAILURE);
    }

    if(sem_close(semIDF) != 0){
        perror("Can't close semaphore");
        exit(EXIT_FAILURE);
    }

    if(sem_unlink(SEMAPHORE_NAME_EMPTIES) != 0){
        perror("Can't delete semaphore");
        exit(EXIT_FAILURE);
    }

    if(sem_unlink(SEMAPHORE_NAME_FULLS) != 0){
        perror("Can't delete semaphore");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}