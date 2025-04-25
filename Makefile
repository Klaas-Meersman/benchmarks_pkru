OPT ?= -O3

CFLAGS   = -std=gnu11 $(OPT) -Wall -Wextra -Wpedantic -Wstrict-aliasing -static

EXEC=pkru pkru_debug pkru_extensive test_MPK_performance test_RW_performance mprotect add_tag_write gem5_forge_tagged_addr PKRUoverride_pkey_tag

all: $(EXEC)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
	
clean: 
	@rm -f $(EXEC)

cleanall: clean