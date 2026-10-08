# Dependency and OpenSSL exposure

Versions below are recipe pins at the audit base, not the host's installed
versions. `depends/packages/packages.mk` determines active recipes. A concern
marked **unassessed** is a gap, never a statement that a library has no CVEs.
Priorities reflect reachable input and compatibility cost, not library age.

| dependency | current version | used by | runtime exposure | known concern | compatibility risk | recommended action | priority |
|---|---|---|---|---|---|---|---|
| OpenSSL | 1.0.1k | core RNG/util, historical scrypt; Qt payment certificates/TLS; tests | crypto in headless; attacker-controlled certificates/HTTPS in GUI | certificate-verification DoS CVE-2015-0286 affects this pin; exact GUI exploit not executed | MEDIUM patch line; HIGH major change/RNG replacement | prioritize certificate/TLS mitigation; no blind bump | P0 |
| Berkeley DB | 4.8.30 | wallet builds | local wallet/environment files, wallet operations through authenticated RPC | local corruption/recovery surface; exhaustive CVE applicability unassessed | HIGH: wallet compatibility | retain 4.8; test corruption, recovery and old wallets before patches | P1 |
| Boost | 1.70.0 | core, filesystem/thread/system/test | indirect parsed paths and threading; not an HTTP server framework here | no concrete reachable vulnerability established in used components | MEDIUM | keep pin; component-specific review, not all-Boost CVE matching | P2 |
| libevent | 2.1.11-stable | daemon/GUI HTTP RPC/REST, Tor control; CLI HTTP client | socket/HTTP input; bind/allowlist configuration matters | no complete vulnerability triage; TLS integration explicitly disabled | MEDIUM | retain pending targeted HTTP/connection/shutdown tests | P1 |
| ZeroMQ | 4.3.1 | optional notification publishers | configured ZMTP listeners accept network connections | CVE-2019-13132 CURVE path is disabled in depends; this is not proof all ZMTP paths are safe | MEDIUM | review enabled NULL/PLAIN/session metadata separately; avoid public binds | P1 |
| MiniUPnPc | 2.0.20180203 | optional UPnP discovery/mapping | LAN discovery and router XML responses | untrusted LAN parser; complete advisory applicability unassessed | MEDIUM | retain opt-in/configuration boundary; targeted parser/backport review | P1 |
| protobuf | 2.6.1 | Qt payment protocol | payment-request bytes | message-size/resource bounds and old parser; no specific CVE asserted | MEDIUM | test malformed/oversized requests; inspect with BIP70 decision | P0 |
| Qt | 5.9.7 | GUI, network, image/font/platform support | payment URLs/files, images, certificates, local GUI | old TLS/parser stack; module-specific CVE mapping incomplete | HIGH major; MEDIUM narrow patch | minimize enabled modules; prioritize payment request exposure | P0 |
| zlib | 1.2.11 | Qt | compressed resources/network content through enabled Qt paths | CVE-2018-25032 concerns deflate; remote reachability not demonstrated | LOW/MEDIUM | assess call paths and upstream fix 5c44459 separately | P1 |
| Expat | 2.2.6 | Linux fontconfig dependency | local font configuration XML | CVE-2022-43680 external-entity/OOM path needs configuration-level reachability verification | MEDIUM | verify fontconfig use; targeted parser update review | P1 |
| FreeType | 2.7.1 | Qt fonts | font files selected by GUI/fontconfig | hostile font parser surface; CVE sweep unassessed | MEDIUM | verify installed binary's actual provider, then patch | P1 |
| fontconfig | 2.12.1 | Linux GUI | font configuration/cache paths | local file trust and XML exposure | MEDIUM | inspect system/static boundary; no blind bump | P2 |
| D-Bus | 1.10.18 | Linux Qt integration | local desktop bus | bus peer trust; full advisory triage unassessed | MEDIUM | test actual Qt D-Bus features and host library linkage | P2 |
| libxcb | 1.10 | Linux Qt platform | X server replies | X server is a trust boundary; parser sweep unassessed | MEDIUM | inspect actual runtime library; harden separately | P2 |
| libX11 | 1.6.2 | Linux Qt | X server replies | X protocol parsing, actual host may supply runtime library | MEDIUM | verify release/runtime provider | P2 |
| libXau | 1.0.8 | Linux X authentication | local X authority files | local file input, not P2P | LOW/MEDIUM | retain pending exact call-path finding | P2 |
| libXext | 1.3.2 | Linux Qt/X | X extension replies | local display server trust | MEDIUM | inspect runtime provider | P2 |
| qrencode | 3.4.4 | GUI | generates QR from application data | encoder, not an untrusted QR decoder | LOW | no justified version change found | P2 |
| xcb_proto | 1.10 | build generator | build only | Python helper compatibility already maintained | LOW | retain | P2 |
| xproto | 7.0.26 | build headers | no standalone runtime code | build input integrity | LOW | retain | P2 |
| xextproto | 7.3.0 | build headers | no standalone runtime code | build input integrity | LOW | retain | P2 |
| xtrans | 1.3.4 | X build support | compiled into X stack where used | platform transport assumptions | MEDIUM | evaluate with X stack | P2 |
| libICE | 1.0.9 | dormant recipe | not selected by current packages.mk | not a current automatic runtime dependency | LOW | retain dormant status; no gratuitous update | P2 |
| libSM | 1.2.2 | dormant recipe | not selected by current packages.mk | same | LOW | retain dormant status | P2 |
| native_protobuf | 2.6.1 | build-time protoc | repository .proto input | build tool only | MEDIUM if target/compiler versions diverge | retain matching target version | P2 |
| native_ccache | 3.3.4 | build acceleration | build environment/cache | cache/source integrity, no shipped service | LOW | isolate build caches | P2 |
| native_cctools / LLVM | 807d6fd1… / 3.7.1 | macOS cross-build | build only | downloaded toolchain/SDK trust | MEDIUM | retain validated tools; deterministic-build audit separately | P2 |
| native_cdrkit | 1.1.11 | macOS image packaging | build file tree | build-only, GCC compatibility patch already present | LOW | retain focused CI | P2 |
| native_libdmg-hfsplus | 0.1 | macOS packaging | build image input | build-only parser/tool trust | LOW/MEDIUM | keep source/hash verification | P2 |
| native_biplist | 0.9 | macOS packaging | build plist inputs | build-only Python tooling | LOW | retain unless reproduction identifies issue | P2 |
| native_ds_store | 1.1.2 | macOS packaging | build .DS_Store inputs | build-only | LOW | retain | P2 |
| native_mac_alias | 2.0.6 | macOS packaging | build aliases | build-only | LOW | retain | P2 |
| vendored LevelDB | source snapshot in src/leveldb | chain/index databases | disk state influenced by validated chain input | recovery/corruption and resource handling not fully audited here | HIGH for storage behaviour | no wholesale replacement | P1 |
| vendored secp256k1 | source snapshot in src/secp256k1 | key/signature validation | consensus-critical signatures and private keys | cryptographic assurance requires dedicated equivalence tests | HIGH | keep; dedicated cryptographic review | P1 |
| vendored UniValue | 1.1.3 | JSON RPC | authenticated/public-policy JSON inputs | parsing depth/size robustness needs dedicated fuzzing | MEDIUM | targeted parser tests | P1 |
| embedded crypto/Allium/scrypt | repository source | PoW/hash validation | block headers | consensus-critical, not a depends version upgrade | HIGH | explicitly outside modification scope | P0 invariant |

## OpenSSL: evidence and component boundaries

`src/Makefile.am` declares SSL+crypto for daemon and CLI, crypto for tx and the
consensus library. Qt and test/benchmark make fragments also declare it. Static
archive selection can omit unused SSL objects despite these linker flags.
`depends/packages/openssl.mk` uses `no-shared`; `libevent.mk` uses
`--disable-openssl`; Qt uses `-openssl-linked`.

Linux release inspection: `readelf -d` reports no dynamic libssl/libcrypto for
`garlicoind`, `garlicoin-cli`, `garlicoin-tx`, or `garlicoin-qt`. All four contain
OpenSSL 1.0.1k component version strings. `libbitcoinconsensus.so` also contains
OpenSSL component strings and exported crypto symbols. This is positive evidence
of embedded crypto, not evidence of a TLS listener. TLSv1.2 strings are present
in Qt but absent in the three command-line executables. Exact SSL object retention
requires an unstripped binary or link map; those were not supplied in the release
artifact. Unit/benchmark linkage is source-evidenced only, not runtime-inspected.
Windows/macOS binary linkage is still unverified.

Important packaging detail: this Linux Qt executable dynamically requires X11,
XCB, fontconfig and FreeType. Recipe pins alone therefore do not certify the GUI's
complete runtime stack on the user's machine; distro security updates matter too.

Source call paths:

* `src/random.cpp`: RAND_add/RAND_bytes; `src/util.cpp`: CRYPTO_num_locks,
  CRYPTO_set_locking_callback, OPENSSL_no_config, RAND_cleanup and Windows RAND_screen.
  Wallet-disabled builds still need core RNG. Removal is not a wallet toggle.
* `src/crypto/scrypt.cpp`: SHA256_Init/Update/Final inside HMAC/PBKDF2. The historical
  scrypt branch remains part of block validation. Do not remove it because current
  mining uses Allium.
* `src/key.cpp`: secp256k1 and local DER import/export, not OpenSSL d2i_ECPrivateKey.
  Do not assign every OpenSSL private-key-parser CVE to wallet.dat handling.
* `src/qt/paymentrequestplus.cpp`: d2i_X509, X509_verify_cert, X509_get_pubkey,
  EVP_sha1/sha256, EVP_Verify*. `paymentserver.cpp` loads roots and uses Qt network
  requests. Malicious request/certificate input is a plausible GUI attack path;
  a working exploit was not tested.
* `src/httpserver.cpp` rejects `-rpcssl`; RPC/REST use plain libevent HTTP, not
  OpenSSL TLS. Network exposure is still important, but TLS-server-only CVEs are
  not automatically daemon RPC vulnerabilities. P2P is not TLS either.

## Specific OpenSSL concern triage

| Concern | Applicability to the pin | Garlicoin reachability assessment |
|---|---|---|
| CVE-2015-0286 certificate algorithm comparison crash, fixed 1.0.1m | affected | candidate GUI certificate-verification path; no PoC executed |
| CVE-2015-0209 d2i_ECPrivateKey use-after-free, fixed 1.0.1m | affected library | no matching wallet import call found; not established reachable through wallet import |
| CVE-2015-1793 alternative-chain verification bypass | introduced 1.0.1n, fixed 1.0.1p | **not applicable to 1.0.1k**; do not inflate findings from age alone |
| SSLv2/SSLv3/DTLS-specific issues | build-dependent | these protocols disabled by recipe; no daemon TLS service |
| Other TLS/certificate issues after 1.0.1k | not exhaustively enumerated | unresolved GUI hardening obligation, not assumed safe |

## Options (none applied in this PR)

1. Keep 1.0.1k temporarily for reproducible headless baseline, with GUI certificate
   exposure explicitly unresolved. Not an acceptable claim of long-term security.
2. Backport narrowly applicable certificate/parser fixes with regression vectors.
   MEDIUM risk; patch maintenance burden must be recorded.
3. Evaluate 1.0.1u as a bounded compatibility experiment. MEDIUM risk and still
   obsolete; do not call it a supported final solution or assume all CVEs vanish.
4. Remove redundant `SSL_LIBS` from headless links after map/symbol verification.
   LOW risk, but core `CRYPTO_LIBS` remains required. Removing payment protocol
   would change a GUI feature and needs its own deliberate decision; disabling
   BIP70 alone may not remove all Qt HTTPS/TLS exposure.
5. Maintained-major OpenSSL upgrade or replacing the RNG/SHA256 consumers: HIGH
   risk, separate portability and cryptographic-equivalence work, especially the
   historical PoW path. No automatic dependency bump here.

Primary advisory sources checked 2026-10-08:

* [OpenSSL 1.0.1 vulnerability catalogue](https://www.openssl-library.org/news/vulnerabilities-1.0.1/index.html)
* [ZeroMQ CVE-2019-13132 upstream issue](https://github.com/zeromq/libzmq/issues/3558)
* [zlib deflate advisory and fix](https://github.com/madler/zlib/issues/605)
* [Expat external entity/OOM bug](https://github.com/libexpat/libexpat/issues/649)

This table does not substitute for a complete advisory feed scan or fuzz campaign.
