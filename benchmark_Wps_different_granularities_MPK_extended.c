#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <err.h>
#include <unistd.h>
#include <sys/mman.h>

//#define TOTAL_WRITES (33554432L)  // 0.33 billion writes
#define TOTAL_WRITES (524288L)  // 0.5 million writes

int main(void)
{
    int64_t pkey_trusted_zone;
    int status;
    size_t pagesize = getpagesize();
    printf("MPK extended benchmark\n");

    printf("Allocating a page of memory: the trusted zone\n");
    void *trusted_zone = mmap(NULL, pagesize, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (trusted_zone == MAP_FAILED)
        err(EXIT_FAILURE, "mmap");

    printf("Writing initial data to the trusted zone...\n");
    ((int *)trusted_zone)[0] = 1001;

    printf("Allocating protection key...\n");
    pkey_trusted_zone = pkey_alloc(0, 0);
    if (pkey_trusted_zone == -1)
        err(EXIT_FAILURE, "pkey_alloc");
    printf("pkey_trusted_zone = %ld\n", pkey_trusted_zone);

    printf("Assigning pkey_trusted_zone to page...\n");
    status = pkey_mprotect(trusted_zone, pagesize,
                           PROT_READ | PROT_WRITE, pkey_trusted_zone);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_mprotect");

    uintptr_t trusted_zone_addr = (uintptr_t)trusted_zone;
    uintptr_t SET_PKRU_override = (1UL << 60); //OR THIS
    uintptr_t tagging_mask = (pkey_trusted_zone << 56); // OR THIS

    int numbers[2] = {
        89,21
    };

    printf("Revoking access to the trusted zone...\n");
    status = pkey_set(pkey_trusted_zone, PKEY_DISABLE_ACCESS | PKEY_DISABLE_WRITE);
    if (status)
        err(EXIT_FAILURE, "pkey_set");

    struct timespec start_time, end_time;

    for (long writes_grouped = 2;
         writes_grouped <= TOTAL_WRITES / 2;
         writes_grouped *= 2)
    {
        printf("Benchmarking writes with writes_grouped: %ld\n", writes_grouped);

        clock_gettime(CLOCK_MONOTONIC, &start_time);
        long writes_done = 0;
        long wrpkru_done = 0;

        // Outer loop — for each batch
        for (long i = 0; i < TOTAL_WRITES; i += writes_grouped)
        {

            // Perform exactly writes_grouped writes
            for (long w = 0; w < writes_grouped; w++)
            {
                int idx = numbers[w % 2];
                ((int *)((trusted_zone_addr | SET_PKRU_override) | tagging_mask))[idx] = idx + i;
                writes_done++;
            }

            wrpkru_done++;
        }

        clock_gettime(CLOCK_MONOTONIC, &end_time);

        double elapsed_time_s =
            (end_time.tv_sec - start_time.tv_sec) +
            (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

        double writes_per_sec = (double)TOTAL_WRITES / elapsed_time_s;

        double elapsed_time_ms = (elapsed_time_s * 1000.0);

        printf("Time taken: %.6f miliseconds\n", elapsed_time_ms);
        printf("Writes per second: %.2f\n", writes_per_sec);
        printf("Total writes done: %ld\n", writes_done);
        printf("Total wrpkru done: %ld\n", wrpkru_done);
        printf("wrpkru per ms: %.2f\n",
               wrpkru_done / (elapsed_time_ms / 1000.0));
        printf("--------------------------------------------------\n");
    }

    pkey_free(pkey_trusted_zone);
    munmap(trusted_zone, pagesize);

    return 0;
}
