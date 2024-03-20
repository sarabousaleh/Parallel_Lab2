#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_BUCKETS 5
#define NUM_THREADS 5
#define ARRAY_SIZE 100

int array[ARRAY_SIZE];
int segment_size = ARRAY_SIZE / NUM_THREADS;

typedef struct {
    int* bucket;
    int count;
} Bucket;

Bucket buckets[NUM_BUCKETS];

void* sort_bucket(void* arg) {
    Bucket* bucket = (Bucket*)arg;
    // Simple insertion sort for the bucket
    int i, j, key;
    for (i = 1; i < bucket->count; i++) {
        key = bucket->bucket[i];
        j = i - 1;

        while (j >= 0 && bucket->bucket[j] > key) {
            bucket->bucket[j + 1] = bucket->bucket[j];
            j = j - 1;
        }
        bucket->bucket[j + 1] = key;
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int i, j;
    struct timespec start, end;
    double computation_time, communication_time, total_time;

    // Initialize array with random numbers
    srand(time(NULL));
    for (i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 1000; // Adjust range as needed
    }

    // Initialize buckets
    for (i = 0; i < NUM_BUCKETS; i++) {
        buckets[i].bucket = (int*)malloc((ARRAY_SIZE / NUM_BUCKETS) * sizeof(int));
        buckets[i].count = 0;
    }

    // Distribute array elements into buckets
    for (i = 0; i < ARRAY_SIZE; i++) {
        int index = i % NUM_BUCKETS; // Distribute elements evenly among buckets
        buckets[index].bucket[buckets[index].count++] = array[i];
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create threads to sort each bucket
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, sort_bucket, (void*)&buckets[i]);
    }

    // Join threads
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    // Merge sorted buckets back into the original array
    int idx = 0;
    for (i = 0; i < NUM_BUCKETS; i++) {
        for (j = 0; j < buckets[i].count; j++) {
            array[idx++] = buckets[i].bucket[j];
        }
        free(buckets[i].bucket);
    }

    // Calculate execution time
    computation_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    communication_time = computation_time; // Assuming communication time is negligible
    total_time = computation_time + communication_time;

    // Output results
    printf("Total Execution Time: %.6f seconds\n", total_time);
    printf("Computation Time: %.6f seconds\n", computation_time);
    printf("Communication Time: %.6f seconds\n", communication_time);

    return 0;
}
