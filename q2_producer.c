// macros that define the semaphore names
#define SEMAPHORE_NAME_EMPTIES "/ourSemaphore1.dat"
#define SEMAPHORE_NAME_FULLS "/ourSemaphore2.dat"

// structure to store messages in shared memory
#define TEXT_SZ 2048
struct shared_use_st {
    char some_text[TEXT_SZ];
};

// import all the required libaries
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <stdio.h>
#include <fcntl.h>
#include <semaphore.h>

int main(){
    int running = 1;
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    char buffer[BUFSIZ];
    int shmid;

    // get the semaphores
    sem_t *semIDE;
    sem_t *semIDF;
    semIDE = sem_open(SEMAPHORE_NAME_EMPTIES, 0);
    semIDF = sem_open(SEMAPHORE_NAME_FULLS, 0);

    // check for errors in semaphore creation
    if (semIDE == SEM_FAILED) {
    perror("Can't open semaphore");
    exit(EXIT_FAILURE);
    }
    if (semIDF == SEM_FAILED) {
    perror("Can't open semaphore");
    exit(EXIT_FAILURE);
    }

    // create or get the shared memory segment
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    if (shmid == -1) {
    fprintf(stderr, "shmget failed\n");
    exit(EXIT_FAILURE);
    }

    // attach the shared memory segment so it can be used by a program
    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE);
    }
    printf("Memory attached at %p\n", shared_memory);

    shared_stuff = (struct shared_use_st *) shared_memory;

    // loop while the user does not input end
    while(running) {
        // decremnet the empty semaphore (init value 1)
        if(sem_wait(semIDE) !=0)
        perror("sem_wait failed in producer");

        // send input message to the shared memory segment
        printf("Enter some text: ");
        fgets(buffer, BUFSIZ, stdin);
        strncpy(shared_stuff->some_text, buffer, TEXT_SZ);

        // if the user enters "end" break the loop
        if (strncmp(buffer, "end", 3) == 0) {
            running = 0;
        }

        // increment the full semaphore (init value 0)
        if(sem_post(semIDF)!=0)
        perror("sem_post failed in producer");
    }

    // detach the shared memory segment
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}