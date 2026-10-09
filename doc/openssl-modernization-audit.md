# OpenSSL modernization audit (2026-10-09)

Status: **blocked prerequisite audit; no OpenSSL upgrade delivered**.
Base: `master` at `bcd2edb371d4d53dc860c390930ec77f592e9ce9`.
Scope: Garlicoin Core 0.18.2; Qt remains 5.9.7; no code, recipes,
consensus, wallet, BDB, network or file-format changes.

## Decision

A single OpenSSL recipe bump is not safe. The pinned Qt 5.9.7 OpenSSL
backend uses opaque OpenSSL 1.0 structures directly and cannot compile
unchanged against 1.1.1 or 3.x. Retrofitting its TLS backend or upgrading Qt
requires work outside the explicitly frozen Qt scope. **Stop condition
reached before implementation.**

OpenSSL 1.1.1w is a useful compatibility test/possible transitional target,
not a maintained long-term security destination. Public 1.1.1 support ended
2023-09-11. A supported 3.x line (e.g. 3.5 LTS) deserves a separately scoped
evaluation; choosing it does not require blindly rewriting all crypto to EVP,
but neither source nor full platform compatibility is established here.

## Dependency and linker map

- `depends/packages/openssl.mk`: 1.0.1k, SHA256
  `8f9faeaebad088e772f4ef5e38252d472be4d878c6b3a2718c10a4fcebe7a41c`.
  `no-shared` produces static libcrypto/libssl archives on Linux, MinGW
  and Darwin. Platform targets are linux-x86_64, linux-generic32/64,
  mingw/mingw64, darwin64-x86_64-cc.
- `depends/packages/packages.mk` always includes openssl.
- `depends/packages/qt.mk`: 5.9.7; depends on openssl/zlib;
  `-static -openssl-linked`; network is built.
- `configure.ac:945-974`: pkg-config libssl/libcrypto or header/library
  fallback checks; flags exported to makefiles. These detect presence,
  not full API compatibility. `EVP_MD_CTX_new` declaration probe at
  1016 remains after the BIP70 implementation was removed.
- `src/Makefile.am`: daemon and CLI list SSL_LIBS + CRYPTO_LIBS;
  tx and libbitcoinconsensus list CRYPTO_LIBS. Qt, Qt tests, unit tests
  and benchmarks also list these libraries in their include makefiles.
  The fuzzy target lists CRYPTO_LIBS.
- Libraries appear after application objects/internal archives.
  In Qt the QT_LIBS precede SSL_LIBS and CRYPTO_LIBS; libssl precedes
  libcrypto. Do not arbitrarily reorder static archives.
- Normal system-dependency builds can instead link dynamically.
  Static depends recipes do not imply every system build is static.

## Direct source use and reachability

| Area / files | Actual use | Compatibility / exposure |
|---|---|---|
| `src/random.cpp` | RAND_bytes, RAND_add | Core entropy/key generation; check return values; no TLS/P2P parsing |
| `src/util.cpp:103-148` | CRYPTO locking callbacks, OPENSSL_no_config, RAND_cleanup; RAND_screen on Windows | Old setup/cleanup; in 1.1.1 locking and RAND_cleanup are compatibility no-ops; configuration suppression maps to OPENSSL_init_crypto(NO_LOAD_CONFIG) |
| `src/crypto/scrypt.cpp`, scrypt-sse2.cpp | SHA256_Init/Update/Final, SHA256_CTX; SSE2 path calls shared PBKDF2 | Used by historical block PoW (`src/primitives/block.cpp`); consensus-sensitive output, do not replace algorithms |
| `src/qt/winshutdownmonitor.cpp` | RAND_event for Windows messages | Deprecated but still declared in 1.1.1 Windows headers; Windows runtime/build remains untested |
| `src/secp256k1/src/tests.c`, bench_verify.c | Optional OpenSSL EC/ECDSA cross-checks and benchmark | ECDSA_SIG->r/->s incompatible with opaque 1.1+ structs; production signing/verification uses libsecp256k1 |
| `src/test/crypto_tests.cpp` | openssl/aes.h and evp.h includes | No actual AES_* or EVP_* calls found; tests exercise internal AES/hash implementations |
| `src/wallet/crypter.cpp` | Internal AES256CBC and SHA512 derivation | EVP_BytesToKey reference is explanatory; no OpenSSL EVP wallet encryption |
| `src/init.cpp`, qt/rpcconsole.cpp | crypto header / attribution | No direct cryptographic call found there |
| QtNetwork (external Qt sources) | SSL, certificate, EVP and EC APIs | Built TLS backend remains despite BIP70 removal; GUI default QSslConfiguration initializes SSL-related code |

SHA256_CTX stays concrete in 1.1.1; the existing direct SHA256 calls remain
available. SHA512/AES elsewhere in Garlicoin use its internal crypto code.
Allium has its own implementations. There is no direct Garlicoin SSL_*
connection handling or EVP_* operation in the active source outside the
optional upstream tests. P2P and HTTP RPC use their existing socket/libevent
stack, not an OpenSSL TLS server.

### After BIP70 removal

`paymentrequestplus`, certificate verification and protobuf payment protocol
implementation are absent. `PaymentServer::LoadRootCAs` and PaymentACK
submission are no-ops; legacy BIP70 files and URI inputs are rejected.
There is no QNetworkAccessManager request path found in active Garlicoin
GUI sources. A QNetworkProxy configuration helper remains.
`qt/bitcoin.cpp:578-580` still creates a default QSslConfiguration and sets
TlsV1_0OrLater. QtNetwork SSL support is compiled and must remain functional
under the task constraints; reduced application reachability does not make
its incompatible backend removable in this batch.

Dead dependency/probe candidates: configure EVP_MD_CTX_new detection,
protobuf discovery and Qt-related protobuf depends wiring, unused includes,
LoadRootCAs shim. None are changed here.

No OPENSSL_VERSION_NUMBER compatibility guard was found in active
Garlicoin source/build m4. The old bitcoin.diff is historical, not build input.
Qt has a narrow BN_is_word version guard, which does not solve its opaque
X509/EVP_PKEY/EVP_CIPHER_CTX uses.

## Reproduced blockers

Verified official Qt archive:
`qtbase-opensource-src-5.9.7.tar.xz`, SHA256
`36dd9574f006eaa1e5af780e4b33d11fe39d09fd7c12f3b9d83294174bd28f00`.

OpenSSL 1.1.1w archive SHA256:
`cf3098950cb4d853ad95c0841f1f9c6d3dc102dccfcacd521d93925208b76ac8`.

Expressions taken from Qt `src/network/ssl/qsslkey_openssl.cpp:279`
and `qsslcertificate_openssl.cpp:69,93`:

```cpp
#include <openssl/evp.h>
#include <openssl/x509.h>
void probe(X509 *x509) {
    EVP_CIPHER_CTX ctx;
    (void)x509->sha1_hash;
    (void)x509->cert_info->version;
}
```

`g++ -I<openssl-1.1.1w>/include -fsyntax-only probe.cpp` fails:

```text
error: aggregate 'EVP_CIPHER_CTX ctx' has incomplete type and cannot be defined
error: invalid use of incomplete type 'X509' {aka 'struct x509_st'}
```

The same expressions fail with system OpenSSL 3.0.13. These are focused
source-expression reproductions, not claims that a full Qt build was run.
Qt also accesses EVP_PKEY internals and old cipher context cleanup APIs;
fixing just the configure check or one expression cannot fix the backend.

`src/secp256k1/build-aux/m4/bitcoin_secp.m4:51` uses
`(void)sig_openssl->r`. Reproduction against 1.1.1w fails with
`invalid use of incomplete type ECDSA_SIG`.
Auto-detection can consequently omit OpenSSL EC cross-checks silently;
that must be repaired with accessors (e.g. ECDSA_SIG_get0) before an upgrade,
not treated as a successful build by disabling those tests.

Passing the existing openssl.mk no-* options to 1.1.1w Configure exits 255:

```text
Unsupported options: no-gmp, no-static_engine, no-store, no-sha0,
no-libunbound, no-rsax, no-krb5, no-jpake
```

Its source has no Makefile.org for the current preprocessing command.
The new unified build/install recipe needs adaptation, DESTDIR staging,
build-info timestamp auditing, platform compiler/AR/RANLIB settings,
and explicit feature/security-option mapping. Do not drop all current
no-* controls wholesale to make Configure pass.

## Version comparison

| Criterion | 1.1.1w transitional candidate | Supported 3.x candidate (e.g. 3.5 LTS) |
|---|---|---|
| Core changes | Linux RAND/SHA/setup sample works unchanged; optional EC test accessors required; Windows deprecated APIs need verification | Linux RAND/SHA sample works with deprecated SHA warnings on 3.0.13; does not prove 3.5; RNG/configuration and deprecated Windows/EC APIs need audit |
| ABI | Not ABI-compatible with 1.0.x; rebuild every consumer | Not ABI-compatible with 1.x; rebuild every consumer |
| Qt 5.9.7 | Opaque-structure blocker | Same blocker plus newer TLS/provider/default-policy differences |
| depends | Unified build/option/staging/determinism changes | Same classes of change plus provider/module packaging and feature checks |
| Linux | Standalone static libraries built; full Core/Qt not built | API sample only against installed 3.0.13; no depends or full build |
| Windows MinGW | Target/cross environment and static OS-library dependencies need validation | Same plus API/provider handling; not validated |
| macOS | Existing SDK 10.11/deployment 10.8 cross toolchain needs actual build; not assumed compatible | Existing toolchain fit unknown; stop if major upgrade required |
| Determinism | Must remove/normalize build timestamps and host paths, stage libraries only, compare repeated builds | Same; ensure intended providers are bundled/initialized without user config changes |
| Regression risk | Smaller API bridge, but Qt work still exceeds scope and public support ended | Wider behavior/security-policy validation; no evidence a wholesale EVP rewrite is mandatory |
| Later Qt 5.15.19 | Meets its >=1.1.1 prerequisite; does not solve XCB/macOS/Qt patch gates | Meets version floor in principle; exact Qt TLS backend must be tested, not assumed from the floor alone |

Recommendation: do not select/ship a production version in this diagnostic
PR. Evaluate a supported 3.x line after authorizing the Qt prerequisite
scope; retain 1.1.1w as a constrained comparison/bridge option only if its
support/security tradeoff is explicitly accepted.

## Litecoin Core 0.18.1 comparison

Reference tag v0.18.1 resolves to
`81c4f2d80fbd33d127ff9b31bf588e4925599d79`.

| Finding | Status |
|---|---|
| OpenSSL 1.0.1k pin and static recipe | Already shared; Garlicoin lacks some extra upstream architecture mappings |
| Qt 5.9.7 backend | Same fundamental dependency; not a 1.1-compatible replacement |
| RAND_screen/RAND_event | Still present upstream; not a ready-made modern RNG fix |
| Locking/configuration integration in random.cpp | Upstream implementation differs; selective candidate only, no RNG forward-port |
| secp ECDSA_SIG internals and configure probe | Same incompatibility present upstream |
| BIP70 EVP_MD_CTX_new/free conditional fallback | Upstream has it; no longer applicable to removed Garlicoin paymentrequestplus |
| Daemon/CLI SSL_LIBS | Litecoin uses CRYPTO_LIBS without SSL_LIBS; candidate separate cleanup after symbol/link-map proof |

No large upstream code import is warranted or included.

## Published 0.18.2 binary inspection

Downloaded Linux TAR, Windows ZIP and macOS TAR from the tagged release.
All three hashes match the published SHA256SUMS (integrity against the
release manifest, not an independent publisher signature).

| Artifact | Observed linkage |
|---|---|
| Linux daemon/CLI/tx/Qt | readelf DT_NEEDED contains no libssl/libcrypto; all contain the 1.0.1k version string |
| Linux libbitcoinconsensus.so | Contains 1.0.1k version string; exported SHA/OpenSSL symbols demonstrate retained static crypto |
| Windows daemon/CLI/tx/Qt and consensus DLL | objdump PE imports contain no OpenSSL DLL; all contain 1.0.1k version string |
| macOS daemon/CLI/tx/Qt | Parsed Mach-O dylib load commands: no external libssl/libcrypto |
| macOS libbitcoinconsensus dylib | Contains 1.0.1k version string and SHA256 symbol names |

This corroborates static depends linkage. Stripped macOS executables have
no OpenSSL version string/symbol names, so the precise embedded version
in those executables is not independently proven by this inspection;
the recipe and companion consensus dylib identify the expected baseline.
otool and MinGW compiler are unavailable locally; Mach-O load-command
parsing and GNU objdump PE inspection were used instead.

Do not infer exact retained libssl object sets from archive link flags,
absence of shared dependencies, or version strings. That needs unstripped
symbols/link maps. GUI binaries retain SSL_CTX_new strings; core does not
have direct TLS calls. No upgraded Garlicoin binary exists.

## Focused validation and gates

- PASS: 1.1.1w standalone `Configure linux-x86_64 no-shared no-dso`,
  `make -j4 build_libs`. This is **not** a migrated depends build.
- PASS: statically linked 1.1.1w API sample exercising existing RAND,
  SHA256, locking/config suppression/cleanup interfaces; reports
  `OpenSSL 1.1.1w 11 Sep 2023`. readelf has only libc.so.6 dependency;
  nm confirms retained RAND_bytes/SHA256_Init/OpenSSL_version.
- PASS with deprecation warnings: equivalent sample against system 3.0.13.
- EXPECTED FAIL: Qt opaque expressions against 1.1.1w and 3.0.13.
- EXPECTED FAIL: secp ECDSA_SIG field probe against 1.1.1w.
- EXPECTED FAIL: old recipe options against 1.1.1w Configure.
- PASS: published baseline archive hashes and three-platform binary inspection.

Not run after the stop: migrated depends, full Linux Core/garlicoin-qt,
crypto unit suite, wallet/P2P smoke, Windows build, macOS deterministic
build, full CI, Release Validation. No green upgrade/regression claim.
Documentation path is outside existing heavy CI path filters; no manual
heavy workflows were dispatched.

## Minimal next variants

1. **Qt-preserving compatibility backport**: separately authorize the Qt
   OpenSSL 1.1 backend backport and validate its size/security provenance.
   This changes Qt sources/patches and is outside this task.
2. **Coordinated Qt/OpenSSL batch**: after the existing Qt platform
   prerequisite work, upgrade Qt and one evaluated OpenSSL version together,
   fixing only required Core/test/build interfaces. Linux focused tests first,
   then Windows/macOS, then release linkage and regression gates.
3. Remain on the baseline temporarily. This provides no security upgrade.
   OpenSSL 1.0.2 is not an answer to the Qt 5.15.19 >=1.1.1 requirement.

Do not split two incompatible OpenSSL ABIs into one executable, disable TLS,
drop EC cross-checks, alter PoW SHA operations, or expand the toolchain
silently. No merge recommended as a completed dependency upgrade.

## Primary references

- [Qt 5.9.7 archive](https://download.qt.io/archive/qt/5.9/5.9.7/submodules/qtbase-opensource-src-5.9.7.tar.xz)
- [OpenSSL 1.1.1w source](https://www.openssl.org/source/old/1.1.1/openssl-1.1.1w.tar.gz)
- [OpenSSL 1.1.1 EOL](https://openssl-library.org/post/2023-09-11-eol-111/)
- [OpenSSL release support policy](https://www.openssl-library.org/policies/releasestrat/)
- [OpenSSL 3 migration guide](https://docs.openssl.org/3.5/man7/ossl-guide-migration/)
- [Litecoin Core 0.18.1](https://github.com/litecoin-project/litecoin/tree/v0.18.1)
- [Garlicoin 0.18.2 release](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/releases/tag/v0.18.2)
