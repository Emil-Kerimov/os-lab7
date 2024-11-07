#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

double a = 0.0;
double b = 2.0;
int num_threads;
double result = 0.0;
pthread_mutex_t mutex;

double f(double x) {
    return 4.0 - x * x;
}

void *calculate_integral(void *arg) {
    int thread_id = *(int *)arg;
    double h = (b - a) / num_threads;
    double local_a = a + thread_id * h;
    double local_b = local_a + h;
    double local_result = 0.0;
    int n = 1000;
    double step = (local_b - local_a) / n;

    for (int i = 0; i < n; i++) {
        double x = local_a + i * step + step / 2;
        local_result += f(x) * step;
    }

    pthread_mutex_lock(&mutex);
    result += local_result;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        num_threads = atoi(argv[1]);
    } else {
        num_threads = 4;
    }

    pthread_t threads[num_threads];
    int thread_ids[num_threads];
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, calculate_integral, &thread_ids[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    printf("Approximate integral: %f\n", result);
    return 0;
}
