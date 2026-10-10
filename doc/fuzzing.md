Fuzz-testing Garlicoin Core
===========================

Garlicoin Core includes the `test/test_garlicoin_fuzzy` fuzzing target. The target is built as a non-installed test program and can be used with AFL-compatible instrumentation.

Build with AFL
--------------

Install AFL or AFL++ using your operating system's package manager, then configure the tree with the AFL compiler wrappers. For example:

```sh
./configure --disable-ccache --disable-shared --enable-tests \
    CC=afl-clang-fast CXX=afl-clang-fast++
make -C src test/test_garlicoin_fuzzy
```

`ccache` is disabled for instrumented builds so normal cached objects are not mixed with fuzz-instrumented objects.

Run
---

Create input and output directories and provide a small corpus appropriate for the parser or data path being exercised:

```sh
mkdir -p inputs outputs
afl-fuzz -i inputs -o outputs -- src/test/test_garlicoin_fuzzy
```

The exact AFL/AFL++ options depend on the installed version and host system. Follow the documentation shipped with the fuzzer for current tuning, persistent-mode, memory-limit, and kernel recommendations.

Source of truth
---------------

The current fuzz target definition is in `src/Makefile.test.include`, and its implementation is in `src/test/test_bitcoin_fuzzy.cpp`. The historical source filename is retained from upstream; the built target in this repository is `test_garlicoin_fuzzy`.
