#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>

#define ITERATIONS 100000000  // Adjust this to get ~10 seconds on your system

int main(void) {
    size_t pagesize = getpagesize();
    int *page1 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    int *page2 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page1 == MAP_FAILED || page2 == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Initialize pages
    for (size_t i = 0; i < pagesize / sizeof(int); ++i) {
        page1[i] = (int)i;
        page2[i] = (int)i;
    }

    printf("Starting %d iterations of random reads/writes...\n", ITERATIONS);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (size_t i = 0; i < ITERATIONS; ++i) {
        int which = rand() % 2;
        int *page = (which == 0) ? page1 : page2;
        size_t offset = rand() % (pagesize / sizeof(int));
        if (rand() % 2) {
            // Write
            page[offset] = (int)i;
        } else {
            // Read
            volatile int val = page[offset];
            (void)val;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Simulation complete.\n");
    printf("Total time: %.6f seconds\n", elapsed);
    printf("Total iterations: %d\n", ITERATIONS);

    munmap(page1, pagesize);
    munmap(page2, pagesize);

    return 0;
}
