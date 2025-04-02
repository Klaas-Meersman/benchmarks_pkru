#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    size_t page_size = getpagesize();
    // Allocate a page-aligned memory region with read/write permissions
    void *buffer = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Change permissions to read-only using mprotect
    if (mprotect(buffer, page_size, PROT_READ) == -1) {
        perror("mprotect");
        return 1;
    }

    // Attempt to write to the read-only memory (triggers segmentation fault)
    *((int *)buffer) = 42;

    munmap(buffer, page_size);
    return 0;
}