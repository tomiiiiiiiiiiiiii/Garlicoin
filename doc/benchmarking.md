Benchmarking
============

Garlicoin Core includes an internal benchmarking framework for selected cryptographic, data-structure, mempool, wallet, and script-validation code paths.

Building
--------

Benchmarks are enabled by default in the maintained build configuration. Build Garlicoin Core normally, or explicitly keep benchmarks enabled with:

```sh
./configure --enable-bench
make
```

Running benchmarks
------------------

Run the benchmark binary from the repository root:

```sh
src/bench/bench_garlicoin
```

To display the available benchmark options:

```sh
src/bench/bench_garlicoin -?
```

The exact set of benchmark cases can change with the source tree. Treat the output of the currently built binary as the source of truth rather than relying on a static list in this document.

When comparing results, use the same compiler, build options, CPU power/performance settings, and background workload. Benchmark numbers from different machines or materially different toolchains are not directly comparable.
