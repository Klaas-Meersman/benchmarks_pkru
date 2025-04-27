#define _GNU_SOURCE
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>


int main(void) {
    int64_t pkey1;
    int status;
    size_t pagesize = getpagesize();

    printf("Allocating a page of memory...\n");
    void *page1 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page1 == MAP_FAILED)
        err(EXIT_FAILURE, "mmap page1");

    printf("Writing initial data to page1...\n");
    ((int*)page1)[0] = 1001;

    printf("Allocating protection key...\n");
    pkey1 = pkey_alloc(0, 0);
    if (pkey1 == -1)
        err(EXIT_FAILURE, "pkey_alloc 1");
    printf("pkey1 = %ld\n", pkey1);

    uintptr_t tagging_mask = (pkey1 << 56); // Use bit 56 for demonstration
    uintptr_t page1_addr = (uintptr_t)page1;

    printf("Assigning pkey1 to page...\n");
    status = pkey_mprotect(page1, pagesize, PROT_READ | PROT_WRITE, pkey1);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_mprotect page1");

    printf("----------------------\n");
    printf("access page1 with memory tag without setting the correct PKRU registers: should succeed with pkruOverride set\n");
    printf("Original: %p\nTagged: %p\n", page1, (void *)(page1_addr | tagging_mask));

    printf("Reading page1 with tag (should succeed): %d\n", ((int *)(page1_addr | tagging_mask))[0]);
    printf("Writing page1 with tag(should succeed)...\n");
    ((int *)(page1_addr | tagging_mask))[0] = 1111;
    printf("Succeeded: page1 now contains: %d\n", ((int *)(page1_addr | tagging_mask))[0]);
    printf("----------------------\n");


    printf("-----------------------\n");
    printf("setting PKRU registers to disable access\n");
    printf("This should have no effect with pkruOverride set as the PKRU registers are ignored.\n");
    printf("Only thing that needs to be correct is the memory tag associated with the pkey of the page.\n");
    status = pkey_set(pkey1, PKEY_DISABLE_ACCESS | PKEY_DISABLE_WRITE); // allow all
    if (status)
        err(EXIT_FAILURE, "pkey_set 1");

    printf("Reading page1 with tag (should succeed): %d\n", ((int *)(page1_addr | tagging_mask))[0]);
    printf("Writing page1 with tag(should succeed)...\n");
    ((int *)(page1_addr | tagging_mask))[0] = 1111;
    printf("Succeeded: page1 now contains: %d\n", ((int *)(page1_addr | tagging_mask))[0]);
    printf("----------------------\n");

    printf("------------------------\n");
    printf("setting PKRU registers to allow access\n");
    status = pkey_set(pkey1, 0); // allow all
    if (status)
        err(EXIT_FAILURE, "pkey_set 2");
    printf("Reading page1 without tag (should fail):");
    printf("%d\n", ((int *)(page1_addr))[0]);
    //should not go past here

    //cleanup anyways
    printf("Freeing pkeys...\n");
    status = pkey_free(pkey1);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_free 1");

    printf("Done.\n");
    exit(EXIT_SUCCESS);
}
