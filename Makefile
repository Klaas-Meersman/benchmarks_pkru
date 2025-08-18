OPT ?= -O3

CFLAGS   = -std=gnu11 $(OPT) -Wall -Wextra -Wpedantic -Wstrict-aliasing -static

EXEC=benchmark_Wps_different_granularities_MPK_baseline benchmark_Wps_different_granularities_MPK_extended m5_bench_WG-1048576_BASE-copy

all: $(EXEC)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
	
clean: 
	@rm -f $(EXEC)

cleanall: clean