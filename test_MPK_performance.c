#define _GNU_SOURCE
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>

#define ITERATIONS 100000000  // 100 million

int main(void) {
    int *page1, *page2;
    int pkey1, pkey2;
    int status;
    size_t pagesize = getpagesize();

    // Allocate two pages of memory
    page1 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page1 == MAP_FAILED)
        err(EXIT_FAILURE, "mmap page1");
    page2 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page2 == MAP_FAILED)
        err(EXIT_FAILURE, "mmap page2");

    // Initialize pages with some data
    page1[0] = 1001;
    page2[0] = 2002;

    // Allocate protection keys for the pages
    pkey1 = pkey_alloc(0, 0);
    if (pkey1 == -1)
        err(EXIT_FAILURE, "pkey_alloc 1");
    pkey2 = pkey_alloc(0, 0);
    if (pkey2 == -1)
        err(EXIT_FAILURE, "pkey_alloc 2");

    // Assign protection keys to the memory pages
    status = pkey_mprotect(page1, pagesize, PROT_READ | PROT_WRITE, pkey1);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_mprotect page1");
    status = pkey_mprotect(page2, pagesize, PROT_READ | PROT_WRITE, pkey2);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_mprotect page2");

    printf("Starting simulation of %d iterations...\n", ITERATIONS);

    // Start timing
    clock_t start = clock();

    for (size_t i = 0; i < ITERATIONS; ++i) {
        // Randomly pick a page and operation
        int which = rand() % 2;       // Select page: 0 for page1, 1 for page2
        int do_write = rand() % 2;    // Select operation: 0 for read, 1 for write
        int rights;

        if (which == 0) {
            // Set PKRU register for page1
            rights = do_write ? 0 : PKEY_DISABLE_WRITE; // Allow write or read-only
            pkey_set(pkey1, rights);

            if (do_write) {
                page1[0] = (int)i; // Write to page1
            } else {
                volatile int val = page1[0]; // Read from page1
                (void)val; // Prevent compiler optimization
            }
        } else {
            // Set PKRU register for page2
            rights = do_write ? 0 : PKEY_DISABLE_WRITE; // Allow write or read-only
            pkey_set(pkey2, rights);

            if (do_write) {
                page2[0] = (int)i; // Write to page2
            } else {
                volatile int val = page2[0]; // Read from page2
                (void)val; // Prevent compiler optimization
            }
        }
    }

    // End timing and calculate elapsed time
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Simulation complete.\n");
    printf("Total time for %d iterations: %.6f seconds\n", ITERATIONS, elapsed);

    // Free protection keys and unmap memory pages
    pkey_free(pkey1);
    pkey_free(pkey2);

    munmap(page1, pagesize);
    munmap(page2, pagesize);

    return 0;
}
