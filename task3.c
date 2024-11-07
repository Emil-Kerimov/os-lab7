#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define DEFAULT_ARRAY_SIZE 10
#define DEFAULT_NUM_READERS 5
#define DEFAULT_NUM_WRITERS 2
#define DEFAULT_RUN_TIME 10

int *array;
int array_size;
int num_readers = DEFAULT_NUM_READERS;
int num_writers = DEFAULT_NUM_WRITERS;
int run_time = DEFAULT_RUN_TIME;

pthread_rwlock_t rwlock;

void* writer_thread(void* arg) {
    while (1) {
        int index = rand() % array_size;
        int value = rand() % 100;

        pthread_rwlock_wrlock(&rwlock);
        array[index] = value;
        pthread_rwlock_unlock(&rwlock);

        usleep(100000);
    }
    return NULL;
}

void* reader_thread(void* arg) {
    while (1) {
        int index = rand() % array_size;

        pthread_rwlock_rdlock(&rwlock);
        int value = array[index];
        pthread_rwlock_unlock(&rwlock);

        usleep(50000);
    }
    return NULL;
}

void* monitor_thread(void* arg) {
    while (1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("Стан масиву: ");
        for (int i = 0; i < array_size; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
        pthread_rwlock_unlock(&rwlock);

        sleep(1);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    array_size = DEFAULT_ARRAY_SIZE;

    int opt;
    while ((opt = getopt(argc, argv, "s:r:w:t:")) != -1) {
        switch (opt) {
            case 's':
                array_size = atoi(optarg);
                break;
            case 'r':
                num_readers = atoi(optarg);
                break;
            case 'w':
                num_writers = atoi(optarg);
                break;
            case 't':
                run_time = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-s array_size] [-r num_readers] [-w num_writers] [-t run_time]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    srand(time(NULL));
    array = malloc(array_size * sizeof(int));
    for (int i = 0; i < array_size; i++) array[i] = 0;

    pthread_rwlock_init(&rwlock, NULL);

    pthread_t readers[num_readers], writers[num_writers], monitor;

    for (int i = 0; i < num_writers; i++) {
        pthread_create(&writers[i], NULL, writer_thread, NULL);
    }
    for (int i = 0; i < num_readers; i++) {
        pthread_create(&readers[i], NULL, reader_thread, NULL);
    }
    pthread_create(&monitor, NULL, monitor_thread, NULL);

    sleep(run_time);

    for (int i = 0; i < num_writers; i++) {
        pthread_cancel(writers[i]);
    }
    for (int i = 0; i < num_readers; i++) {
        pthread_cancel(readers[i]);
    }
    pthread_cancel(monitor);

    pthread_rwlock_destroy(&rwlock);
    free(array);

    return 0;
}
