#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <err.h>
#include <unistd.h>
#include <sys/mman.h>
// #include <gem5/m5ops.h>
// #include <m5_mmap.h>

#define TOTAL_WRITES (2097152L) // 1 million writes

int main(void)
{
    // m5op_addr = 0XFFFF0000;
    // map_m5_mem();

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

    printf("Revoking access to the trusted zone...\n");
    status = pkey_set(pkey_trusted_zone, PKEY_DISABLE_ACCESS | PKEY_DISABLE_WRITE);
    if (status)
        err(EXIT_FAILURE, "pkey_set");

    struct timespec start_time, end_time;
    long writes_grouped = 1048576; ///////---------------------------------> CHANGE THIS
    printf("Benchmarking writes with writes_grouped: %ld\n", writes_grouped);

    long writes_done = 0;
    long wrpkru_done = 0;
    int idx = 0;
    uintptr_t trusted_zone_addr = (uintptr_t)trusted_zone;
    uintptr_t SET_PKRU_override = (1UL << 60);          // OR THIS
    uintptr_t tagging_mask = (pkey_trusted_zone << 56); // OR THIS

    ///////////////////////////////////
    printf("m5 annotation start\n");
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    // m5_work_begin_addr(0, 0);
    //////////////////////////////////

    int *trusted_zone_PKRU_override = (int *)(trusted_zone_addr | SET_PKRU_override);
    uintptr_t override_addr = (uintptr_t)trusted_zone_PKRU_override;

    // Outer loop — for each batch
    for (long i = 0; i < TOTAL_WRITES; i += writes_grouped)
    {

        // Perform exactly writes_grouped writes
        for (long w = 0; w < writes_grouped; w++)
        {
            __asm__ volatile(
                "or %[mask], %[base]\n\t"
                "movl %[data], (%[base], %[index], 4)\n\t"
                : [base] "+r"(override_addr)
                : [mask] "r"(tagging_mask),
                  [data] "r"(idx),      // 32-bit data
                  [index] "r"((long)idx) // 64-bit index
                : "memory");
        }
    }

    ///////////////////////////////////
    // m5_work_end_addr(0, 0);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    printf("m5 annotation end\n");
    //////////////////////////////////

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

    pkey_free(pkey_trusted_zone);
    munmap(trusted_zone, pagesize);

    // unmap_m5_mem();
    return 0;
}