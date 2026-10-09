# Windows MinGW proof of build: Qt 5.15.19 and OpenSSL 3.5.9

Date: 2026-10-09. Windows x86_64 only. This follow-up supersedes the missing-artifact
blocker in `qt515-openssl35-windows-checkpoint.md`; it does not replace the recorded
Linux proof or doublespend classification. No Linux/macOS build or runtime test
was repeated. The user explicitly authorized recovery of lost Windows artifacts.

## Source and environment

Branch: `maintenance/qt515-openssl35`. Dependency recipes: `853842ea48ac`;
Windows static Qt link fix: `01f6518a2059bb8a38a22b59afde28a478609cde`.
Existing commits were preserved. The detached Windows worktree started at
`23e6dde633d121cab679df404a98bd0678bad254` with the exact link fix applied.

Ubuntu 24.04 host, native GCC 13.3, MinGW POSIX GCC 13.2,
binutils 2.41.90.20240122, mingw-w64 11.0.1. This restores the previously recorded
Windows toolchain; it is not a new toolchain migration. Qt alone uses C++17 on
MinGW. Garlicoin uses C++11. No other dependency version was changed.

## Recovery and focused compatibility fixes

All Windows depends, including Qt, BDB 4.8.30.NC and miniupnpc, completed staging
and caching. Qt reports OpenSSL and directly linked OpenSSL enabled; its
"OpenSSL 1.1" configure label describes the API family, not the archive version.
The actual dependency is OpenSSL 3.5.9. TLS sources compiled without a TLS patch.
Accessibility and DirectWrite remain enabled.

An initial parallel recovery produced seven zero-length COFF archive members in
libcrypto (bn_exp, bn_gf2m, rsa_gen, ts_req_utils, ciphercommon_ccm,
ciphercommon_gcm and rsa_kmgmt). The invalid cache/stamps were quarantined.
Only OpenSSL was rebuilt at -j2, with the same recipe and sources; archive
inspection passed and its SHA256 matched the earlier valid two-root proof.
The cause of the initial artifact corruption was not established. It was not
fixed by disabling features or altering OpenSSL/Qt code. Qt's cached negative
configure result was preserved and cleared before completing its build.

```text
fdc1160f3da82a0aab0941a51c6e52c7cdc9d8dc4847a50939b180be9e6e6d38  libcrypto.a
a9fb6ed3f7d881344cb109b10dd618e01c623c7ce1dde3f6e44c5c66b318b717  libssl.a
```

Core's real configure errors identified two missing static link inputs:

- Qt >= 5.15 needs `Qt5WindowsUIAutomationSupport` for qwindows. The module was
  already built and staged; `uiautomation.pri` explicitly declares it.
- Core/Gui/FontDatabaseSupport static pkg-config metadata and qwindows
  `windows.pri` specify mpr, userenv, netapi32, wtsapi32, d3d11, dxgi, dxguid,
  dwrite and d2d1. The manual finder now adds these Windows system libraries.

The added check is limited to Windows Qt >= 5.15. Both qminimal and qwindows
configure link probes pass. No Qt source patch was necessary. Existing Windows
libraries and older Qt finder behavior remain available.

The first make invocation regenerated configure after the m4 edit without the
shell-local CONFIG_SITE and could not find BDB. The resumed command exports the
same depends CONFIG_SITE throughout configure and make; it does not disable the
wallet or use host libraries.

## Commands and evidence

Windows depends:

```sh
TAR_OPTIONS=--no-same-owner make -j4 HOST=x86_64-w64-mingw32 \
  SOURCES_PATH=/workspace/scratch/38f156860da2/Garlicoin/depends/sources \
  BASE_CACHE=/workspace/scratch/38f156860da2/Garlicoin/depends/built
```

Core in `proof/windows-src`:

```sh
export CONFIG_SITE="$PWD/depends/x86_64-w64-mingw32/share/config.site"
./configure --enable-wallet --with-gui=qt5 --enable-tests \
  --enable-gui-tests --enable-openssl-tests --disable-bench
make -j4 -C src V=1
```

`ENABLE_OPENSSL_TESTS` is defined in the secp configuration. Unit, Qt GUI and
OpenSSL EC cross-check test executables remain enabled. Build success is not a
claim that Windows runtime tests passed.

## Validation

| Platform / test | Result |
|---|---|
| Windows depends / Qt 5.15.19 | PASS, build/stage/cache |
| Windows Core with wallet | PASS, make exit 0, 6m23.720s |
| garlicoind / cli / tx / Qt GUI | PASS, x86_64 PE executables |
| libbitcoinconsensus | PASS, x86_64 DLL |
| Core unit / fuzzy / GUI test builds | PASS |
| secp tests including OpenSSL EC cross-checks / exhaustive build | PASS, 4.274s |
| NSIS installer build | PASS, unsigned; two existing wizard BMP format warnings |
| PE imports / static OpenSSL linkage | PASS, all ten executable/DLL artifacts inspected |
| Windows test execution / GUI startup / RNG runtime | Not run; native Windows runner unavailable |
| Full Windows release reproducibility | Not established |
| Linux / macOS / CI matrix | Not rerun / not started |

`qt515-openssl35-windows-validation.txt` contains the selected evidence, binary
hashes and import lists. Raw retained logs, maps and binaries are in the proof
artifact archive. The redirected Core log is partial (its tail ends during
compilation); successful command completion and all final PE artifacts were
observed. The focused map/trace captures are complete.

## Final linkage and runtime assets

All ten application/library/test artifacts are PE32+ x86_64, with no import of
OpenSSL, Qt, libstdc++, libgcc or libwinpthread DLLs. The GUI statically contains
qwindows, TLS QtNetwork and OpenSSL 3.5.9. Map-assisted links of existing objects
into separate proof copies (no source recompilation) resolve libssl/libcrypto
from the Windows depends prefix, with the hashes listed above. CLI/daemon link
commands present libssl too, but their final symbol sets use libcrypto without
TLS; garlicoin-tx links libcrypto. libbitcoinconsensus contains no OpenSSL symbols
and imports only ADVAPI32, KERNEL32 and msvcrt. No OpenSSL 1.0.1k version string
was found; absence of a version string alone was not used as proof.

Both default and legacy provider entry points are built into libcrypto and the
linked applications. A built-in legacy provider is not evidence that it is
required or automatically selected. No runtime provider DLLs, engines or OpenSSL
configuration are installed. `no-module no-dso no-engine no-autoload-config`
remains in the unchanged OpenSSL recipe. Embedded `/etc/ssl` and
`/lib/ossl-modules` metadata strings remain; they do not override these build
options or imply successful external loading. Certificate trust discovery by
Qt/Windows is separate from provider/config loading. RNG symbols RAND_bytes,
RAND_event and RAND_screen link successfully and use the existing Windows
CryptoAPI backend. Runtime RNG/TLS validation on Windows is still outstanding.

## Reproducibility and limits

Recovered OpenSSL archives exactly match the saved valid two-prefix hashes;
GNU archive member timestamps and the Qt RCC deterministic flags are retained.
Full Qt/Core/installer byte reproducibility has not been tested. This ordinary
cross-build has current-time PE headers and absolute host paths in assertion
strings (including pre-existing Boost headers and Qt headers). The observations
are documented rather than hidden. It is not a deterministic release build;
no claim of unchanged release reproducibility or release readiness is made.
Release-pipeline validation remains a separate gate.

The only added implementation change is the Windows Qt link finder. Consensus,
PoW/scrypt, blockchain/block/transaction formats, P2P/network, wallet/BDB formats,
addresses, keys, datadir and RPC semantics remain unchanged. No production
signing/verification or secp test source changed during Windows recovery. There
was no toolchain scope expansion, new dependency version or TLS backport.


## Packaging and durable resume state

The existing NSIS recipe was exercised through its same install/strip/makensis
steps, using copies of daemon, CLI and Qt binaries. The installer is an unsigned
PE32 NSIS wrapper carrying x86_64 application payloads. Two warning 5040 messages
refer to the existing `nsis-wizard.bmp` format; no GUI/resource redesign was made.
Installer execution on Windows has not been validated. The normal package still
includes daemon, CLI and GUI; the separate proof archive also includes tx,
consensus DLL and all compiled test executables.

Proof binaries report build suffix `23e6dde63-dirty`: they were built from the
recorded checkpoint plus exactly the subsequently committed m4 link fix. They
are review artifacts, not signed release binaries. No recompilation was done
merely to change this suffix.

Durable non-source artifacts:

- `Garlicoin-Windows-depends-cache-20261009.tar.gz`: completed verified Windows
  package caches (final replacement supersedes the invalid initial recovery).
- `Garlicoin-Windows-proof-20261009.tar.gz`: original application and test PE
  files, consensus DLL, stripped packaging copies, unsigned installer, retained
  raw logs, PE manifests and focused linker maps/traces/commands.

The Git branch/commits and this report preserve source state. The former
checkpoint remains historical; its missing-state blocker is resolved. Restore
caches with the matching recorded toolchain before requesting another build.
The first remaining validation step is native Windows runtime smoke/tests,
followed by separately authorized release reproducibility/CI. Neither requires
repeating Linux's completed doublespend classification. macOS remains deferred.

Archive SHA256:

```text
65e924c17e44569d5a480e5f0fcd4e98bfd14ac16fd34f14fa781408f087e33e  Garlicoin-Windows-depends-cache-20261009.tar.gz
dec145edcd159ac09c4a8f81657a5ef211bd74a634cdb7ce3419418867683be2  Garlicoin-Windows-proof-20261009.tar.gz
```

Both artifact archives were transferred successfully before this checkpoint was
closed. The Windows source worktree was then aligned to `01f6518a2` without
rebuilding binaries. Only generated autoconf backups/confdefs remain untracked.
