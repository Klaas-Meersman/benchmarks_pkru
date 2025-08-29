#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    // Get the system page size
    size_t pagesize = sysconf(_SC_PAGESIZE);

    // Allocate one page of memory, readable and writable
    void *addr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Change protection to read-only
    if (mprotect(addr, pagesize, PROT_READ) != 0) {
        perror("mprotect");
        exit(1);
    }
    printf("Memory protection changed to read-only.\n");
    printf("Attempting to write to read-only memory...\n");
    // Attempt to write to the now read-only memory (will cause segmentation fault)
    ((char*)addr)[0] = 'X';

    // Cleanup (not reached)
    munmap(addr, pagesize);
    return 0;
}
