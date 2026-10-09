# Qt 5.15.19 upgrade preflight (2026-10-09)

Status: **blocked before implementation**. This branch retains the working Qt
5.9.7 dependency recipe, all existing patches and GUI code. No dependency,
consensus, wallet, RPC, P2P, datadir or GUI behaviour changes are made.

Baseline: master `bcd2edb371d4d53dc860c390930ec77f592e9ce9`.

The requested Qt-only upgrade cannot preserve the current linked TLS setup:
Qt 5.15.19 explicitly requires OpenSSL >= 1.1.1, whereas depends pins 1.0.1k.
The task explicitly forbids upgrading OpenSSL and requires stopping when another
critical dependency change is necessary.

## Verified source

Downloaded and extracted the official open-source qtbase archive:

- URL: https://download.qt.io/archive/qt/5.15/5.15.19/submodules/qtbase-everywhere-opensource-src-5.15.19.tar.xz
- SHA256: `51e91c73abacab81e64efd01bf95794e79c0e605ad80947a024769f0dd620a32`
- The downloaded archive hash matches the official adjacent `.sha256` file.
- The 5.15 archive naming adds `everywhere-` to the current suffix.
- qttools and qttranslations also need version-specific URLs/hashes before an
  implementation. Their archives/hashes were not validated in this stopped preflight.

Evidence below refers to files in that archive, not a newer Core recipe.

## Blocking TLS incompatibility

Current `depends/packages/qt.mk`:

- `_version=5.9.7`
- `_dependencies=openssl zlib`
- `-openssl-linked`, static libraries, system zlib

Current `depends/packages/openssl.mk`: `_version=1.0.1k`.

Qt 5.15.19 `src/network/configure.json`, library `openssl_headers`:

```c
#if !defined(OPENSSL_VERSION_NUMBER) || OPENSSL_VERSION_NUMBER-0 < 0x10101000L
#  error OpenSSL >= 1.1.1 is required
#endif
```

The linked OpenSSL library test inherits this header test. A local
`c++ -fsyntax-only` reproduction using the upstream OpenSSL_1_0_1k
`crypto/opensslv.h` and this exact Qt version guard exits 1 with:

```text
error: #error OpenSSL >= 1.1.1 is required
```

This is a reproduced version gate, not a completed Qt configure/build.
Using host OpenSSL would bypass deterministic target dependencies and does not
validate the pinned cross-build. Merely relaxing the guard is not an API port.

Disabling Qt TLS is a separate behavioural decision, not an automatic workaround:
`src/qt/bitcoin.cpp` still includes and calls QSslConfiguration/ QSsl APIs.
BIP70 removal does not alone prove that an SSL-disabled Qt is equivalent.
Runtime-loaded OpenSSL still requires compatible headers and would add a runtime
dependency. Linking two OpenSSL generations statically into one executable also
requires a symbol/linkage audit; adding a second Qt-only OpenSSL is not a trivial
isolated solution.

## Additional platform work identified

### Linux

Qt 5.15.19 no longer offers the old bundled `-qt-xcb` and
`-qt-xkbcommon-x11` configuration. Its GUI configure describes XCB >= 1.11,
xkbcommon >= 0.5.0 and external xcb utility libraries; Garlicoin currently pins
libxcb 1.10 and does not declare that complete dependency set.

Relevant locations: Qt `src/gui/configure.json`, `config_help.txt`;
Garlicoin `depends/packages/qt.mk`, `libxcb.mk`,
`build-aux/m4/bitcoin_qt.m4`.
The current static plugin link check still requests `-lqxcb -lxcb-static`;
the new external XCB linkage needs to be validated and adapted.

This is evidence for prerequisite changes, not proof of a successful or failed
full Linux build. No XCB dependencies were changed.

### macOS

Qt `mkspecs/common/macx.conf` sets deployment target 10.13 and
`QT_MAC_SDK_VERSION_MIN = 10.14`. Garlicoin uses deployment target 10.8,
SDK 10.11 and a legacy cross-qmake spec/cctools stack.
These defaults expose a platform-support mismatch requiring explicit SDK,
deployment-target and toolchain validation. This audit does not claim that
changing only the SDK is sufficient, nor that every older target necessarily
fails. No SDK/toolchain or support policy was changed.

### Windows MinGW

The linked OpenSSL gate also applies to MinGW. Existing POSIX MinGW/static plugin
configuration must be tested after prerequisites are resolved. No Windows build
was attempted and no successful Qt 5.15.19 Windows build is claimed.

## Patch inventory

All old files remain unchanged. Dry-run checks used the unmodified 5.15.19
qtbase tree (`patch --dry-run --batch --forward -p2`).

| Existing file | Verified 5.15.19 finding | Future action |
| --- | --- | --- |
| fix_rcc_determinism.patch | Original hunk does not apply; RCC already contains QT_RCC_SOURCE_DATE_OVERRIDE and SOURCE_DATE_EPOCH handling. | Drop after checking timestamp precedence/reproducibility in the real build. |
| fix_numeric_limits_compile_error.patch | Old corelib/tools path moved to corelib/text; header already includes limits. | Drop when implementing 5.15.19. |
| fix_riscv64_arch.patch | Original hunk does not apply; bundled double-conversion already handles __aarch64__ and __riscv. | Drop when implementing 5.15.19. |
| fix_configure_mac.patch | sdk.prf infoarg fix already present; configure sysroot hunk still applies in dry-run. | Split out the obsolete hunk; validate remaining cross-SDK workaround. |
| fix_qt_pkgconfig.patch | Condition changed; original hunk fails. Internal modules are still excluded by default. | Adapt only as required by Garlicoin static pkg-config detection. |
| fix_no_printer.patch | Cocoa header hunk applies; plugins.pro hunk fails because its context changed. | Re-evaluate disabled-printer build before removal or rebasing. |
| xkb-default.patch | qtConfTest_xkbConfigRoot is absent; original hunk fails. | Re-evaluate reproducible XKB configuration in external xkbcommon dependencies. |
| mac-qmake.conf | Copied configuration, not a unified diff; old SDK/deployment/toolchain assumptions remain. | Rework only after selecting/validating macOS prerequisites. |

A failed patch dry-run is not by itself evidence that the original fix is
unnecessary. No patch was removed solely because it failed to apply.

## Modules and build wiring

Current application pkg-config detection requires Qt5Core, Qt5Gui, Qt5Network
and Qt5Widgets; Qt5Test supports GUI tests and Qt5DBus is optional.
Depends builds corelib/network/widgets/gui/plugins/testlib and native
qttools lrelease/lupdate plus qttranslations. QML, Quick and WebEngine are
not required by the inspected build wiring.

Future work must preserve host-tool bootstrapping, target flags, static platform
plugins and deterministic resource handling; audit configure options individually.
No GUI source changes have been implemented or demonstrated necessary beyond
the discussed consequences of intentionally disabling SSL.

## Validation and next decision

Completed: source download/hash check, source inspection, seven patch dry-runs,
and the reproduced OpenSSL version-gate compilation failure.

Not run: complete Qt build, garlicoin-qt build, Linux/Windows/macOS CI,
GUI tests or unit/functional smoke tests with Qt 5.15.19.
There is no upgraded binary to test. Heavy CI was deliberately not launched on
a dependency configuration known to violate the source requirements.

Minimal choices:

1. Keep Qt 5.9.7 for now, preserving the validated release baseline.
2. Approve a separate prerequisite project: audit/modernize OpenSSL linkage,
   then resolve XCB dependencies and macOS SDK/toolchain/support requirements;
   follow with the Qt upgrade and focused/full cross-platform validation.
3. Separately investigate SSL-disabled Qt after BIP70 removal, explicitly
   authorizing the GUI TLS behaviour change; this still leaves XCB and macOS work.

Recommendation: option 2 if Qt 5.15.19 is the goal; keep the existing release
until those prerequisites and all requested build/test gates pass. Do not merge
this diagnostic PR as if it delivered Qt 5.15.19.
