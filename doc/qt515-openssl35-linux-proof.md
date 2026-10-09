# Qt 5.15.19 / OpenSSL 3.5.9 Linux proof

Date: 2026-10-09. Base: master `bcd2edb371d4d53dc860c390930ec77f592e9ce9`.
Scope: Linux only, per the user's follow-up. Windows/macOS and release-wide
validation are deferred. This is not a cross-platform release or merge approval.

## Source verification and TLS proof

PR #42 and #43 still describe the current base correctly: Qt 5.9.7 and
OpenSSL 1.0.1k are pinned; BIP70 is absent, while RNG, historical scrypt SHA256
and QtNetwork remain OpenSSL consumers. Production ECDSA stays in libsecp256k1.

Official archives downloaded from the publishers; local SHA256 matched each
publisher's adjacent checksum file:

| Archive | SHA256 |
| --- | --- |
| qtbase-everywhere-opensource-src-5.15.19.tar.xz | `51e91c73abacab81e64efd01bf95794e79c0e605ad80947a024769f0dd620a32` |
| qttools-everywhere-opensource-src-5.15.19.tar.xz | `2694af886130d63b8a42bcd138f4035fd8108273ebd16781f87300da352f1aa6` |
| qttranslations-everywhere-opensource-src-5.15.19.tar.xz | `967e0d259af13f47e853f183cffc9671cc054380188fee28402f6425132a2c7b` |
| openssl-3.5.9.tar.gz | `603f5602e2eef00d77fbd429d34dcd5822bb301757a1bc9cdb24c670f1eb859a` |

Qt's `src/network/configure.json` requires OpenSSL >= 1.1.1. More importantly,
its SSL symbol wrappers contain explicit OpenSSL 3 branches for
SSL_get1_peer_certificate / EVP_PKEY_get_base_id and opaque structure accessors.
An unmodified upstream QtCore/QtNetwork static build succeeded with 3.5.9.
The resulting probe reports build/runtime 3.5.9, initializes the default provider
and RAND_bytes, and connects to a loopback TLS server with certificate and
hostname verification. No ignoreSslErrors or security-level reduction is used.
A second link against the actual depends OpenSSL archives passes with nonexistent
OPENSSL_CONF/OPENSSL_MODULES paths, using the same OPENSSL_no_config initialization
as Core. Proof-only Qt .prl linkage metadata was redirected to those archives;
no TLS source was patched. There is no Qt TLS compatibility backport.

## OpenSSL recipe and providers

The recipe uses the unified Configure/build_libs/install_dev system, explicit
CC/CXX/AR/RANLIB, DESTDIR staging, libdir=lib, no-shared, no-module, no-dso,
no-engine and no-autoload-config. It retains supported old algorithm/protocol
exclusions rather than passing obsolete 1.0 options. Headers/static archives/
pkg-config metadata are installed; executables, dynamic libraries and provider
module files are not required. Upstream tests are not configured out.

The default provider is built into libcrypto and automatically supplies EVP/RNG/
TLS algorithms. The legacy provider is also compiled in by no-module, but is not
loaded by Core/Qt and is not required by the inspected application paths.
OpenSSL 3's default policy does not automatically enable legacy-only ciphers.
Core's existing OPENSSL_no_config call suppresses even Qt's explicit configuration
load request; no external openssl.cnf is needed. Existing 1.1+ locking/cleanup
compatibility macros remain; production crypto/RNG/SHA call sites are unchanged.

SOURCE_DATE_EPOCH=1 controls upstream build metadata. A small metadata-only patch
normalizes the depends prefix in compiler-info strings; ENGINESDIR/MODULESDIR
are constant paths and DSO/module loading is disabled. Two builds in different
source/build/install directories produced byte-identical archives:

- libcrypto.a: `f9f1d9f9ad50c88bcdcb4ebacbea0f1b465aeecfa1e3833d7fb151b3ec42b040`
- libssl.a: `8637e6816fef661e6faef50cda4c4afde0f9a81c67fc62924997e4cf5ed3f521`

This proves static OpenSSL archive reproducibility for this toolchain, not full
Qt/Core or release-package reproducibility.

## Qt patch classification

| Old patch/config | Action and evidence |
| --- | --- |
| fix_rcc_determinism.patch | Removed: upstream RCC has both QT_RCC_SOURCE_DATE_OVERRIDE and SOURCE_DATE_EPOCH handling; the latter takes precedence. |
| fix_numeric_limits_compile_error.patch | Removed: relocated qbytearraymatcher header already includes limits. |
| fix_riscv64_arch.patch | Removed: double-conversion already recognizes aarch64/riscv. No RISC-V build claimed. |
| xkb-default.patch | Removed: old qtConfTest_xkbConfigRoot is absent; external xkbcommon is configured with /usr/share/X11/xkb explicitly. |
| fix_qt_pkgconfig.patch | Adapted from Bitcoin v25.0 to preserve internal static-module pkg-config files. |
| fix_configure_mac.patch | Upstream sdk.prf infoarg hunk removed; remaining sysroot hunk retained and dry-run checked. macOS unvalidated. |
| fix_no_printer.patch | Cocoa include retained; plugins.pro removal rebased onto the new styles context. Linux build exercised; Cocoa unvalidated. |
| mac-qmake.conf | Retained pending separate macOS SDK/toolchain work; no macOS support-policy change. |

Unknown old configure options no-qml-debug and no-xinput2 are removed because
their old option names do not exist in qtbase 5.15. No QML module is built.
Qt 5.15's XInput configuration is upstream-controlled. TLS remains linked.
QtDBus must be built/staged before QtThemeSupport; no new dbus version is selected.

## Linux XCB provenance

External XCB recipes are adapted selectively from Bitcoin Core v25.0,
commit `8105bce5b384c72cf08b25b7c5343622754e7337`, which uses Qt 5.15.
This is a secondary depends reference, not a Core forward-port. Litecoin
v0.21.4 was checked first and still pins Qt 5.9.8, so it cannot supply this set.

- libxcb 1.10 -> 1.14 and xcb-proto 1.10 -> 1.14.1 satisfy Qt's >=1.11 floor.
- New xcb-util 0.4.0, image 0.4.0, keysyms 0.4.0, renderutil 0.3.9,
  wm 0.4.1 and libxkbcommon 0.8.4 (including xkbcommon-x11).
- Existing libX11/libXext/fontconfig/freetype versions are retained.
- Python 3.12 py-compile helper refresh is retained for xcb-proto; the
  xkbcommon GCC 12+ array-bounds workaround comes from the reference recipe.
- bison is a required build tool for xkbcommon's parser, added to Linux GUI
  workflow prerequisites. No unrelated dependency versions are bumped.
- PKG_CONFIG_LIBDIR typo is corrected in config.site, preventing fallback to
  host pkg-config directories in a depends build.

## secp test compatibility

The old explicit --enable-openssl-tests configure fails because ECDSA_SIG is
opaque. Accessor adaptation follows the approach in upstream secp commit
`31abd3ab8d63a1e5623408e5fc73440579456a95`; older system OpenSSL remains supported.
After accessors, the old DER assertion fails on a negative ASN.1 integer:

```
Failure 10 on 30 1b 02 0f be 00 ff ff ff ff ff 00 00 00 00 60 00 00 fe 02 08 00 df 00 ff ff ff 01 00
```

OpenSSL 3.5.9 `crypto/ec/ec_asn1.c` delegates d2i_ECDSA_SIG to
`crypto/asn1_dsa.c`'s positive-integer decoder. For this exact rejected-negative
case the test now independently cross-checks syntax/sign with OpenSSL's general
ASN.1 decoder. Scalar validity, DER roundtrip and actual ECDSA sign/verify
cross-checks remain enabled. The captured negative fixture is a fixed regression
case. No production libsecp code or consensus parsing is changed. The 64-count
standalone suite passes with OpenSSL checks enabled and a retained random seed.

## Validation

Linux distribution: Ubuntu 24.04, GCC 13.3, x86_64-unknown-linux-gnu.
Builds are local focused builds; no cross-platform CI was dispatched.
The build was resumed from the original QtDBus failure, not restarted from a
clean tree. QtDBus was first built in the existing Qt work directory; the
remaining depends build used `qt_qt_libs='corelib dbus network widgets gui plugins
testlib'`, now reflected exactly in qt.mk. Recipe option removals and the plugin
link/include errors were corrected incrementally. No clean rebuild of the final
recipe hashes is claimed.

Commands (from the repository root):

```sh
TAR_OPTIONS=--no-same-owner make -j8 -C depends HOST=x86_64-unknown-linux-gnu NO_UPNP=1
./autogen.sh
CONFIG_SITE="$PWD/depends/x86_64-unknown-linux-gnu/share/config.site" ./configure \
  --enable-wallet --with-gui=qt5 --enable-tests --enable-gui-tests \
  --enable-openssl-tests --without-miniupnpc --enable-sse2 --disable-bench
QT_RCC_SOURCE_DATE_OVERRIDE=1 SOURCE_DATE_EPOCH=1 make -j6
```

TAR_OPTIONS is an ownership workaround for this execution environment. It is
not a source compatibility patch. Berkeley DB remains 4.8.30. UPnP is omitted
from this focused build using the existing supported build option; TLS, wallet,
ZMQ, crypto/EC checks and GUI tests are enabled. Release-wide UPnP validation is
not claimed.

| Linux platform/test | Result |
| --- | --- |
| OpenSSL / Qt / full depends | PASS, incremental build with QtDBus |
| Core: daemon, CLI, tx, consensus library | PASS |
| garlicoin-qt and both test binaries | PASS |
| Full unit suite | NOT GREEN: exit 124 after 240s in miner_tests/CreateNewBlock_validity |
| Other focused unit checks | 251 distinct cases completed without assertion errors across the recorded runs; full suite remains incomplete |
| Crypto | PASS: 18 cases, crypto_tests/scrypt_tests/random_tests/key_tests/wallet_crypto |
| secp check | PASS: tests and exhaustive_tests, zero skips; OpenSSL EC enabled |
| Wallet/RPC/CLI/P2P smoke | PASS: wallet_encryption, rpc_uptime, rpc_net, interface_bitcoin_cli, p2p_transport |
| Existing wallet/datadir roundtrip | PASS: published 0.18.2 -> new Core -> published 0.18.2 |
| Qt URI / nested RPC / endian compatibility | PASS after URI prefix correction |
| Qt WalletTests | NOT GREEN: QtTest's 300s per-function timeout, exit 134 |
| XCB GUI application startup | Environment blocked: Xvfb cannot establish sockets; AF_UNIX creation returns EPERM |
| TLS verified certificate/hostname probe | PASS against static OpenSSL 3.5.9 |
| OpenSSL / RCC focused reproducibility | PASS; full Qt/Core/release reproducibility unproven |
| Linux CI / Release Validation | Not dispatched; local proof only |
| Windows / macOS | Deferred by explicit scope |

The wallet roundtrip used the published Linux archive whose SHA256 matches
SHA256SUMS (`a59698678bd671213976273942695045dd22b99c691cd5e88d7418d43a7f3cd4`).
It verifies legacy-address ownership, old/new message signatures, wallet load,
new address creation, raw transaction creation/signing and cross-check by the
baseline, chain startup and clean shutdown. Its raw transaction prevout is
synthetic: funded broadcast/confirmation is not claimed. A separate attempt to
mine 101 blocks with the published baseline exhausted its tries after height 46;
it did not produce the funded fixture. No user wallet or datadir was accessed.

## Required compatibility changes

- Linux static-plugin configure omits the removed bundled xcb-static archive for
  Qt >= 5.15 while retaining it for older Qt. The real link probes pass.
- The GUI wallet test needs an explicit base58.h include for EncodeDestination.
- Existing `garlicoin://` normalization removed 11 characters although its
  prefix has 12. Correcting that length makes the existing case-sensitive URI
  assertion pass. Normal `garlicoin:` parsing and the application appearance
  are unchanged. This is an existing prefix-length bug discovered by the tests,
  not evidence of an OpenSSL or consensus regression.
- secp changes are confined to its configure EC probe and test source described
  above. No production Core crypto call-site modernization was necessary on Linux.

## Linkage

readelf -d, ldd and objdump -p were checked for all five final ELF artifacts.
Linker -t traces independently identify the exact depends archive paths; their
SHA256 values equal the reproducibility results above. nm confirms retained
OpenSSL 3 API symbols (and SHA256_Init in the consensus library). Version strings
are auxiliary evidence only: dead-section elimination removes version functions
from the non-GUI binaries.

| Artifact | Retained OpenSSL evidence | Dynamic libssl/libcrypto |
| --- | --- | --- |
| garlicoind | depends 3.5.9 libcrypto; RAND_bytes/EVP_MD_fetch/init symbols | None |
| garlicoin-cli | depends 3.5.9 libcrypto; RAND_bytes/EVP_MD_fetch/init symbols | None |
| garlicoin-tx | depends 3.5.9 libcrypto; RAND_bytes/EVP_MD_fetch/init symbols | None |
| garlicoin-qt | depends 3.5.9 libssl/libcrypto; SSL/init symbols; version 3.5.9 | None |
| libbitcoinconsensus.so | depends 3.5.9 libcrypto SHA256 objects; no TLS requirement | None |

Core link commands may scan libssl without retaining TLS objects. These traces
and symbol checks do not assert that every static archive object is included.
No 1.0.1k or host OpenSSL archive appears in the final linker traces.
The GUI retains normal shared dependencies on XCB utilities, xkbcommon,
fontconfig/freetype and the system C/C++ runtime; OpenSSL/Qt remain static.
XKB keyboard data and CA trust stores remain normal OS resources, separate from
OpenSSL provider/config/module files.

## Reproducibility and remaining gates

RCC resource payload and compiled object are identical with different source
roots and input mtimes under SOURCE_DATE_EPOCH=1. Generated C++ comments contain
source paths, but they do not change the compiled object. Retained evidence gives
the SHA256 values. GNU archive member timestamps are deterministic. OpenSSL
configuration/provider loading was exercised with nonexistent paths.

The focused binaries still contain absolute include paths from Boost assertion
strings; Qt embeds its configured installation prefix. Existing release builds
use controlled paths, but full path-independent Qt/Core builds and a second
release-package comparison were not run. A local proof must not be described as
completed deterministic release validation.

The additional run of previously unexecuted suites also timed out after 240s in
`tx_validationcache_tests/tx_mempool_block_doublespend`, before the four final
non-mining suites were run separately and passed. No tests were deleted or
excluded from the configured build. The focused selection is recorded in the
evidence file and must not be described as a full-suite pass.

The unresolved mining gates are in existing master fixtures: miner_tests creates
COINBASE_MATURITY+10 blocks; Qt WalletTests uses TestChain100Setup. Their loops
run the real Garlicoin PoW and GetNextWorkRequired. The published baseline's
101-block RPC attempt also failed to complete its fixture. This is evidence of a
baseline fixture/mining issue, not proof that every timeout has the same cause;
a controlled baseline test comparison is still needed. Do not change consensus,
DGW, scrypt, block validation or security checks to make these tests green.

Recommended next Linux work is a separately focused diagnosis of those baseline
fixtures plus an XCB startup run on an environment supporting Xvfb sockets. The
branch is a buildable checkpoint, not a fully validated migration or release.
No implementation PR is opened at this stage: the full Linux validation gate is
not green and a PR would trigger the deferred Windows/macOS workflows.

No changes were made to consensus, PoW/scrypt, blockchain/network parameters,
block/transaction formats, P2P protocol, addresses/keys, datadir/wallet.dat,
Berkeley DB version or RPC semantics. No TLS/security checks or test targets were
removed or disabled. Windows/macOS toolchain/support policy is unvalidated and
must be reviewed separately before a release.

## Follow-up: doublespend fixture classification (2026-10-09)

The requested isolated `tx_validationcache_tests/tx_mempool_block_doublespend`
comparison is complete. No repository code or timeout settings were changed.
Both executions used an external 1200-second limit, the same environment and
GCC 13 optimization settings, and nonexistent OpenSSL configuration/module paths.
The unchanged master baseline was built separately at
`bcd2edb371d4d53dc860c390930ec77f592e9ce9`.

| Build | Result | Wall time | User CPU | System CPU |
|---|---|---:|---:|---:|
| Qt 5.15.19 / OpenSSL 3.5.9 branch | PASS, exit 0 | 1146.069 s | 1144.192 s | 0.478 s |
| Unchanged master / OpenSSL 1.0.1k | PASS, exit 0 | 839.016 s | 837.460 s | 0.328 s |

The faster baseline was investigated with five alternating pairs of 20,000
scrypt hashes, using the existing production objects, identical inputs and each
build's actual static libcrypto archive. Median times were 10789.772306 ms and
10699.953347 ms respectively (+0.84%, overlapping ranges). Hash checksums were
identical: `28f4361449f89f919546aece355124118cdba212ac7b7779993c156e855a2ee7`.
The production scrypt object text was byte-identical, SHA256
`905d80622a5e6fa2fced7b493286099243115c357e30d5879e6110e53e3486c5`.
Fixture and scrypt source were unchanged. Random coinbase keys change the PoW
nonce trial count, providing a plausible explanation for the full-fixture gap;
one pair does not establish equality of all performance.

Classification: **inherited slow fixture; both builds PASS; no demonstrated
Qt/OpenSSL regression**. This supersedes the pending comparison for this one
test above. Other Linux limitations remain as recorded; Linux was not rerun
for the subsequent Windows proof. Full raw logs are retained outside the source
tree in `proof/logs/doublespend-{modern,baseline}.log`,
`doublespend-comparison-metadata.txt` and `doublespend-scrypt-timing.log`.
