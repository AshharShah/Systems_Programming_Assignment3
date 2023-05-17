// include all the required libararies
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// define the macros to be used
#define BUFFER_SIZE 20
#define NUM_ITEMS 100

// create the shared buffer for both threads
int buffer[BUFFER_SIZE];
int buffer_index = 0;

// create semaphores 
sem_t full_sem, empty_sem;
pthread_mutex_t mutex;

// the producer function
void* producer(void* arg) {
    // generate random items and put into buffer
    int item;
    for (int i = 0; i < NUM_ITEMS; ++i) {
        item = rand() % 100; // Generate a random item

        sem_wait(&empty_sem); // Wait for an empty slot in the buffer
        pthread_mutex_lock(&mutex); // Acquire the mutex

        // Produce item and add it to the buffer
        buffer[buffer_index] = item;
        printf("Produced item: %d\n", item);
        buffer_index++;

        pthread_mutex_unlock(&mutex); // Release the mutex
        sem_post(&full_sem); // Signal that the buffer has one more item
    }

    return NULL;
}

void* consumer(void* arg) {
    int item;

    for (int i = 0; i < NUM_ITEMS; ++i) {
        sem_wait(&full_sem); // Wait for a filled slot in the buffer
        pthread_mutex_lock(&mutex); // Acquire the mutex

        // Consume item from the buffer
        buffer_index--;
        item = buffer[buffer_index];
        printf("Consumed item: %d\n", item);

        pthread_mutex_unlock(&mutex); // Release the mutex
        sem_post(&empty_sem); // Signal that the buffer has one more empty slot
    }

    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;

    // Initialize semaphores and mutex
    sem_init(&full_sem, 0, 0);
    sem_init(&empty_sem, 0, BUFFER_SIZE);
    pthread_mutex_init(&mutex, NULL);

    // Create producer and consumer threads
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // Wait for threads to finish
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // Clean up resources
    sem_destroy(&full_sem);
    sem_destroy(&empty_sem);
    pthread_mutex_destroy(&mutex);

    return 0;
}