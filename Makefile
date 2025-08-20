OPT ?= -O3

CFLAGS   = -std=gnu11 $(OPT) -Wall -Wextra -Wpedantic -Wstrict-aliasing -static

EXEC=benchmark_Wps_different_granularities_MPK_baseline benchmark_Wps_different_granularities_MPK_extended m5_bench_WG-1_BASE m5_bench_WG-1_EXT m5_bench_WG_range_BASE m5_bench_WG_range_EXT m5_bench_WG_range_EXT_TEST_onmypc

all: $(EXEC)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
	
clean: 
	@rm -f $(EXEC)

cleanall: clean