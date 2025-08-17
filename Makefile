OPT ?= -O3

CFLAGS   = -std=gnu11 $(OPT) -Wall -Wextra -Wpedantic -Wstrict-aliasing -static

EXEC=pkru pkru_debug pkru_extensive test_MPK_performance test_RW_performance mprotect add_tag_write gem5_forge_tagged_addr PKRUoverride_pkey_tag PKRU_override_FULL_TEST benchmark_Wps_different_granularities_MPK_baseline benchmark_Wps_different_granularities_MPK_extended m5_annotated_test

all: $(EXEC)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
	
clean: 
	@rm -f $(EXEC)

cleanall: clean