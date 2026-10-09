# Windows MinGW checkpoint and resume blocker

Date: 2026-10-09. Scope: Windows x86_64 MinGW only; Linux results are preserved,
not rerun. macOS remains deferred. No implementation PR, CI matrix or merge was
started.

## Durable source checkpoint

- Branch: `maintenance/qt515-openssl35`.
- Implementation checkpoint: `853842ea48ac0b8cc669b2cbce41b512351e7c0f`.
- Doublespend classification: `378662e9828007700e9da7a71e2880120a76815d`.
- Original baseline: `bcd2edb371d4d53dc860c390930ec77f592e9ce9`.

The implementation commit changes only `depends/packages/qt.mk`:

1. Pass `OPENSSL_LIBS="-L$(host_prefix)/lib -lssl -lcrypto -lws2_32 -lgdi32 -lcrypt32"`
   to the actual Qt configure invocation for MinGW. These are the system
   libraries specified by OpenSSL 3.5.9 `Configurations/10-main.conf`,
   `mingw-common.ex_libs`, also exposed in its static pkg-config metadata.
   Qt's fallback found our archives but omitted these libraries. Applying the
   variable to the recipe's initial separate `export` command did not propagate
   it to configure; the committed recipe applies it directly to configure.
2. Build Qt itself as C++17 only on MinGW. Official Qt 5.15.19
   `src/corelib/text/qlocale_win.cpp:498` uses `std::size`, which fails with the
   previous C++11 setting. No source patch or compiler upgrade was required.
   Garlicoin's C++ standard was not changed; Windows Core has not yet been built.

No Core, RNG, consensus, wallet, network, BDB or test code changed in this stage.
No TLS/security check or Garlicoin test target was disabled.

## Results observed before the stop

This section reconstructs the results retained in the working session. The raw
Windows log files and compiled archives are no longer available; this is not a
replacement for their complete original contents or a fresh build verification.

| Step | Recorded result |
|---|---|
| MinGW POSIX toolchain | GCC 13.2.0, binutils 2.41.90.20240122, mingw-w64 11.0.1 |
| OpenSSL 3.5.9 depends, `mingw64` | Build, staging and caching PASS |
| OpenSSL static archive reproducibility | PASS, two roots/prefixes, byte-identical archives |
| Native ccache/protobuf, Boost, libevent, ZeroMQ, qrencode, protobuf, zlib | Build/staging/cache completed |
| Qt 5.15.19 configure with final MinGW recipe | PASS, OpenSSL=yes, directly linked=yes |
| Qt OpenSSL TLS source files | Compiled without TLS backport |
| Full Qt build/staging/cache | Incomplete: interrupted at user request, exit 130 |
| Remaining depends (including Windows BDB/miniupnpc) | Not completed/verified |
| Windows Core, GUI, unit/GUI/secp test executables | Not started |
| Final PE imports/linkage, installer, Windows runtime tests | Not started |

Compiled Qt TLS files included `qsslsocket_openssl_symbols.cpp`,
`qssldiffiehellmanparameters_openssl.cpp`, `qsslcertificate_openssl.cpp`,
`qsslellipticcurve_openssl.cpp`, `qsslkey_openssl.cpp`,
`qsslsocket_openssl.cpp` and `qsslcontext_openssl.cpp`. The configure summary
label "OpenSSL 1.1" denotes an API family; the actual headers and static
archives were OpenSSL 3.5.9.

`nm` on the completed Windows archive showed `RAND_event`, `RAND_screen`,
`RAND_bytes` and `OpenSSL_version`, with references to Windows
`CryptAcquireContextW` and `CryptGenRandom`. OpenSSL's compatibility functions
call `RAND_poll`; the MinGW entropy implementation uses CryptoAPI. The archive
contained built-in providers and no provider DLL installation. Final application
linkage and runtime RNG behavior remain unverified.

Recorded SHA256 values (identical for both Windows archive builds):

```text
fdc1160f3da82a0aab0941a51c6e52c7cdc9d8dc4847a50939b180be9e6e6d38  libcrypto.a
a9fb6ed3f7d881344cb109b10dd618e01c623c7ce1dde3f6e44c5c66b318b717  libssl.a
```

GNU archive member timestamps were deterministic. Full Qt/Core/release
reproducibility was not claimed.

## Exact interrupted build location

The previous workspace root was `/workspace/scratch/38f156860da2`.

- Main repository: `Garlicoin`, on the implementation branch.
- Detached Windows worktree: `proof/windows-src`, aligned to `853842ea48ac`.
- Partial Qt tree:
  `proof/windows-src/depends/work/build/x86_64-w64-mingw32/qt/5.15.19-bee35036a35`.
- Stamps present: `.stamp_extracted`, `.stamp_preprocessed`,
  `qtbase/.stamp_configured`; no completed `.stamp_built`.
- Last log: `proof/logs/windows-depends-4.log`, 1224 `compiling` lines observed.
  The tail included Windows platform/style compilation and
  `compiling .moc/release/moc_qwindowsxpstyle_p.cpp`.
- Earlier attempts: `windows-depends.log`, `windows-depends-2.log`,
  `windows-depends-3.log` (link flags and C++11 blockers, subsequently resolved).
- Other logs: `windows-autogen.log`, `windows-environment.txt`,
  `windows-qt-config.{log,summary}`, `windows-openssl-inspection.txt`,
  `windows-openssl-repro-{config,build}.log`.
- Windows cache was `Garlicoin/depends/built/x86_64-w64-mingw32`, approximately
  22 MiB; the active Qt build tree was approximately 1.5 GiB.

The interrupted command was:

```sh
cd /workspace/scratch/38f156860da2/proof/windows-src/depends
TAR_OPTIONS=--no-same-owner make -j4 HOST=x86_64-w64-mingw32 \
  SOURCES_PATH=/workspace/scratch/38f156860da2/Garlicoin/depends/sources \
  BASE_CACHE=/workspace/scratch/38f156860da2/Garlicoin/depends/built
```

## Resume check: missing build state

On the subsequent resume request, the workspace contained none of the previous
repository, `proof` directory, raw logs, compiled objects, dependency/source
caches or MinGW installation. The repository was restored from GitHub at
`853842ea48ac`; its tracked state was clean and the two saved commits were
verified. No matching checkpoint archive was found among persistent files.
The planned archive transfer had not completed before the environment reset.
Earlier statements that raw logs were retained refer to the former workspace;
Linux selected evidence and doublespend results remain in the committed Linux
documents, but uncommitted raw logs are unavailable.

The first unfinished logical step remains completion of Qt and Windows depends.
An incremental resume requires restoration of the prior partial Qt tree, caches,
prefix and matching toolchain. Without them, previously completed dependency
and Qt object builds must be regenerated. The user explicitly forbade repeating
completed builds/tests, so no reconstruction build or test was started during
the resume check. Restore the missing state or obtain authorization for the
necessary Windows-only rebuild before executing make. Do not mark the partial
Windows proof green or rerun Linux/macOS as a workaround.

After restoring/regenerating Windows depends, continue with Core configured
with wallet, Qt, unit/GUI tests and secp OpenSSL cross-checks enabled; then build
all Windows artifacts and inspect PE imports/static OpenSSL linkage and
packaging. Final Windows runtime/release validation remains a separate gate.
