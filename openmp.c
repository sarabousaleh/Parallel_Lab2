#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_BUCKETS 5
#define ARRAY_SIZE 100

int array[ARRAY_SIZE];

typedef struct {
    int* bucket;
    int count;
} Bucket;

Bucket buckets[NUM_BUCKETS];

int compare(const void* a, const void* b) {
    return *((int*)a) - *((int*)b);
}

int main() {
    int i, j;
    double start_time, end_time;
    double computation_time, communication_time, total_time;

    // Initialize array with random numbers
    srand(time(NULL));
    for (i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 1000; // Adjust range as needed
    }

    // Initialize buckets
    for (i = 0; i < NUM_BUCKETS; i++) {
        // Calculate the size of each bucket dynamically based on the number of elements it will contain
        int bucket_size = (ARRAY_SIZE / NUM_BUCKETS) + ((i < ARRAY_SIZE % NUM_BUCKETS) ? 1 : 0);
        buckets[i].bucket = (int*)malloc(bucket_size * sizeof(int));
        buckets[i].count = 0;
    }

    // Distribute array elements into buckets
    for (i = 0; i < ARRAY_SIZE; i++) {
        // Calculate the index of the bucket for the current element
        int index = array[i] % NUM_BUCKETS;
        // Add the current element to the corresponding bucket
        buckets[index].bucket[buckets[index].count++] = array[i];
    }

    // Measure execution time
    start_time = omp_get_wtime();

    // Parallel sort each bucket using OpenMP
    #pragma omp parallel for
    for (i = 0; i < NUM_BUCKETS; i++) {
        qsort(buckets[i].bucket, buckets[i].count, sizeof(int), compare);
    }

    // Merge sorted buckets back into the original array
    int idx = 0;
    for (i = 0; i < NUM_BUCKETS; i++) {
        for (j = 0; j < buckets[i].count; j++) {
            array[idx++] = buckets[i].bucket[j];
        }
        free(buckets[i].bucket);
    }

    end_time = omp_get_wtime();

    // Calculate execution time
    total_time = end_time - start_time;

    // Computation time is the time spent sorting
    computation_time = total_time;

    // Communication time is 0 because there's no explicit communication
    communication_time = 0.0;

    // Output results
    printf("Total Execution Time: %.6f seconds\n", total_time);
    printf("Computation Time: %.6f seconds\n", computation_time);
    printf("Communication Time: %.6f seconds\n", communication_time);

    // Calculate communication/computation ratio
    double ratio = communication_time / computation_time;
    printf("Communication/Computation Ratio: %.6f\n", ratio);

    return 0;
}
