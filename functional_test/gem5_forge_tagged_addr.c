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

        
    uintptr_t tagging_mask = (15UL << 56);  // Use bit 56 for demonstration
    uintptr_t original_addr = (uintptr_t)addr;
    uint64_t vaddr = (uint64_t)(original_addr | tagging_mask);
    uint64_t bits64_56 = vaddr >> 56;
    printf("vaddr: 0x%lx, bits64_56: 0x%lx\n", vaddr, bits64_56);
    if (bits64_56 < 0x10 && bits64_56 > 0x00) {
        printf("Found tagged address - vaddr: 0x%lx, tag bit: 0x%lx\n", vaddr, (vaddr >> 56));
    }
    munmap(addr, pagesize);
    return 0;
}