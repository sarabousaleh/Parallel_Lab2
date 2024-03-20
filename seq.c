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
        buckets[i].bucket = (int*)malloc(ARRAY_SIZE * sizeof(int)); // Allocate maximum possible size
        buckets[i].count = 0;
    }

    // Distribute array elements into buckets
    for (i = 0; i < ARRAY_SIZE; i++) {
        // Calculate the index of the bucket for the current element
        int index = array[i] % NUM_BUCKETS;
        // Add the current element to the corresponding bucket
        buckets[index].bucket[buckets[index].count++] = array[i];
    }

   
    start_time = clock();

    // Sequentially sort each bucket
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

    end_time = clock();

    
    total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    
    computation_time = total_time;

    
    communication_time = 0.0;

   
    printf("Total Execution Time: %.6f seconds\n", total_time);
    printf("Computation Time: %.6f seconds\n", computation_time);
    printf("Communication Time: %.6f seconds\n", communication_time);

    
    double ratio = communication_time / computation_time;
    printf("Communication/Computation Ratio: %.6f\n", ratio);

    return 0;
}
