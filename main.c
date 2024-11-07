#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

long shared_variable = 0;
int num_operations = 1000;

void* increment(void* arg) {
    for (int i = 0; i < num_operations; i++) {
        shared_variable++;
    }
    return NULL;
}

void* decrement(void* arg) {
    for (int i = 0; i < num_operations; i++) {
        shared_variable--;
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    int num_pairs = 2;

    int opt;
    while ((opt = getopt(argc, argv, "p:o:")) != -1) {
        switch (opt) {
            case 'p':
                num_pairs = atoi(optarg);
            break;
            case 'o':
                num_operations = atoi(optarg);
            break;
            default:
                fprintf(stderr, "Usage: %s [-p num_pairs] [-o num_operations]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    pthread_t threads[num_pairs * 2];

    for (int i = 0; i < num_pairs; i++) {
        pthread_create(&threads[i * 2], NULL, increment, NULL);
        pthread_create(&threads[i * 2 + 1], NULL, decrement, NULL);
    }

    for (int i = 0; i < num_pairs * 2; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Кількість дій: %d\n", num_operations * num_pairs * 2);
    printf("Початкове значення: 0\n");
    printf("Фінальне значення: %ld\n", shared_variable);

    return 0;
}
