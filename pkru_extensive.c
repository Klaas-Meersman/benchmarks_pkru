#define _GNU_SOURCE
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>


int main(void) {
    int *page1, *page2;
    int pkey1, pkey2;
    int status;
    size_t pagesize = getpagesize();

    printf("Allocating two pages of memory...\n");
    page1 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page1 == MAP_FAILED)
        err(EXIT_FAILURE, "mmap page1");
    page2 = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page2 == MAP_FAILED)
        err(EXIT_FAILURE, "mmap page2");

    printf("Writing initial data to both pages...\n");
    page1[0] = 1001;
    page2[0] = 2002;

    printf("Allocating two protection keys...\n");
    pkey1 = pkey_alloc(0, 0);
    if (pkey1 == -1)
        err(EXIT_FAILURE, "pkey_alloc 1");
    printf("pkey1 = %d\n", pkey1);

    pkey2 = pkey_alloc(0, 0);
    if (pkey2 == -1)
        err(EXIT_FAILURE, "pkey_alloc 2");
    printf("pkey2 = %d\n", pkey2);

    printf("Assigning pkey1 to page1, pkey2 to page2...\n");
    status = pkey_mprotect(page1, pagesize, PROT_READ | PROT_WRITE, pkey1);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_mprotect page1");
    status = pkey_mprotect(page2, pagesize, PROT_READ | PROT_WRITE, pkey2);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_mprotect page2");

    printf("Setting pkey1: allow all access\n");
    status = pkey_set(pkey1, 0); // allow all
    if (status)
        err(EXIT_FAILURE, "pkey_set 1");

    printf("Setting pkey2: disable write\n");
    status = pkey_set(pkey2, PKEY_DISABLE_WRITE);
    if (status)
        err(EXIT_FAILURE, "pkey_set 2");

    printf("Reading page1 (should succeed): %d\n", page1[0]);
    printf("Writing page1 (should succeed)...\n");
    page1[0] = 1111;
    printf("page1 now contains: %d\n", page1[0]);

    printf("Reading page2 (should succeed): %d\n", page2[0]);
    printf("Attempting to write page2 (should fail)...\n");
    fflush(stdout);
    page2[0] = 2222; // This should crash

    // The following lines should not be reached
    printf("page2 now contains: %d\n", page2[0]);

    printf("Freeing pkeys...\n");
    status = pkey_free(pkey1);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_free 1");
    status = pkey_free(pkey2);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_free 2");

    printf("Done.\n");
    exit(EXIT_SUCCESS);
}
