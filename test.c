#define _GNU_SOURCE
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>


int main(void) {
    u_int64_t *page1, *page2;
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
    printf("Address page1 before pkey: %p", (void*)page1);
    printf("Address page2 before pkey: %p", (void*)page2);


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

    printf("Addres s page1 after pkey: %p", (void*)page1);
    printf("Address page2 after pkey: %p", (void*)page2);

}