# Simple Microbenchmarks

This repository contains **functional tests** and **benchmarks** designed for thesis microbenchmarking.

## Directory Structure

- `functional_test/Makefile`: For compiling functional tests.
- `benchmark/Makefile`: For compiling benchmarks with gem5 annotation support.

## Functional Tests

Compile functional tests using the `Makefile`.  
**gem5 libraries are not required.**

**Steps:**
1. Enter the functional tests directory:
    ```
    cd functional_test
    ```
2. Edit `Makefile` to include the desired benchmarks for compilation.
3. Compile:
    ```
    make
    ```

## Benchmarks with gem5 Annotations

Compile benchmarks in the `benchmark` directory using **m5 annotations** for gem5 simulation.  
This allows gem5 to record benchmark statistics outside the simulated OS.

**Preparation:**
- From your gem5 directory, install m5 libraries:
    ```
    scons x86.CROSS_COMPILE=x86_64-linux-gnu- build/x86/out/m5
    ```

**Steps:**
1. Enter the benchmarks directory:
    ```
    cd benchmark
    ```
2. Update your local gem5 path as required in your environment.
3. Insert the desired benchmarks into `Makefile`.
4. Compile:
    ```
    make
    ```

### About m5 Annotation

m5 annotations indicate the start and end of benchmark execution for gem5, allowing accurate statistics to be captured in the host stats file.  
For details, see: [gem5 bootcamp - Accelerating Simulation](https://bootcamp.gem5.org/#02-Using-gem5/08-accelerating-simulation).

---

