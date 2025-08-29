GEM5_PATH = /home/klaas-meersman/2024/gem5
ISA = x86
GXX = g++

all: m5_bench_WG_range_EXT

m5_bench_WG_range_EXT: m5_bench_WG_range_EXT.cpp
	$(GXX) -o m5_bench_WG_range_EXT m5_bench_WG_range_EXT.cpp -no-pie \
	-I$(GEM5_PATH)/include \
	-L$(GEM5_PATH)/util/m5/build/$(ISA)/out \
	-I$(GEM5_PATH)/util/m5/src -lm5

clean:
	rm -f m5_bench_WG_range_EXT