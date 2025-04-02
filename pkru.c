#define _GNU_SOURCE
#include <err.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

int main(void)
{
    int status;
    int pkey;
    int *buffer;

    /*
    * Allocate one page of memory.
    */
    buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (buffer == MAP_FAILED)
        err(EXIT_FAILURE, "mmap");

    /*
    * Put some random data into the page (still OK to touch).
    */
    *buffer = __LINE__;
    printf("buffer contains: %d\n", *buffer);

    /*
    * Allocate a protection key:
    */
    pkey = pkey_alloc(0, 0);
    printf("pkey in binary: %d\n", pkey);
    if (pkey == -1)
        err(EXIT_FAILURE, "pkey_alloc");

    /*
    * Disable access to any memory with "pkey" set,
    * even though there is none right now.
    */
    status = pkey_set(pkey, PKEY_DISABLE_ACCESS);
    if (status)
        err(EXIT_FAILURE, "pkey_set");

    /*
    * Set the protection key on "buffer".
    * Note that it is still read/write as far as mprotect() is
    * concerned and the previous pkey_set() overrides it.
    */
    status = pkey_mprotect(buffer, getpagesize(),
                            PROT_READ | PROT_WRITE, pkey);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_mprotect");

    printf("about to read buffer again...\n");

    /*
    * This will crash, because we have disallowed access.
    */
    printf("buffer contains: %d\n", *buffer);

    status = pkey_free(pkey);
    if (status == -1)
        err(EXIT_FAILURE, "pkey_free");

    exit(EXIT_SUCCESS);
}