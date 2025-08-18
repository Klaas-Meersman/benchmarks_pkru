#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

int main() {
    size_t pagesize = sysconf(_SC_PAGESIZE);
    void *addr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    
    if(addr == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

        
    uintptr_t tagging_mask = (1UL << 56);  // Use bit 56 for demonstration
    uintptr_t original_addr = (uintptr_t)addr;
    
    // Verify canonical address requirements
    if (original_addr & tagging_mask) { //should be 0
        fprintf(stderr, "Original address uses reserved bits\n");
        munmap(addr, pagesize);
        return 1;
    }
    
    void *new_addr = (void *)(original_addr | tagging_mask);
    
    printf("Original: %p\nTagged: %p\n", addr, new_addr);
    
    // Write through original address (safe)
    *(int *)new_addr = 42;

    printf("Value via tag: %d\n", *(int *)new_addr);  // Output: 42 => will fail due to canonical address check. We change our hardware so we use the tag to check rights to access. No 5 level paging needed, we just adjust the TLB.
    
    munmap(addr, pagesize);
    return 0;
}
