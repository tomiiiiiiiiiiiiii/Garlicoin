# Native macOS x86_64 Qt 5.15 / OpenSSL 3.5 checkpoint

Date: 2026-10-09.
Branch: `maintenance/qt515-openssl35`.
This native build supersedes the old Linux-to-Darwin cross-toolchain preflight blocker for this validation route only.

## Environment and dependencies

- GitHub Actions runner: `macos-15-intel`, macOS 15.7.9.
- Xcode 16.0 (16A242d), Apple Clang 16.0.0, macOS SDK 15.0.
- Architecture: x86_64. Deployment target: macOS 10.13.
- Repository depends: Qt 5.15.19, OpenSSL 3.5.9, Berkeley DB 4.8.30.
- Wallet and GUI enabled; BDB version and wallet.dat format unchanged.

## Minimal native build fixes

- BDB's legacy configure probes need Darwin-only warning demotions for implicit int and implicit function declarations with modern Clang.
- Qt 5.15 no longer supplies a separate CglSupport archive; require it only for older Qt.
- Static Cocoa needs CoreVideo, Carbon, QuartzCore and IOSurface frameworks.
- The macOS GUI now imports the already-linked minimal platform plugin alongside Cocoa for the headless startup gate.
- No consensus, PoW, network, P2P, RPC or production behavior changes.

## Verified result: PASS

Build commit: `b1552ca4817158493e7b4f2f2ac5b95b790da792`.
Successful native workflow: https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/runs/37955511711
Artifact: `garlicoin-macos-x86_64-qt515-openssl35` (ID 11628741183).
Download: https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/runs/37955511711/artifacts/11628741183

The artifact contains a tar.gz and its SHA256 file. The tar.gz contains:
`bin/garlicoind`, `bin/garlicoin-cli`, `bin/garlicoin-tx`,
`bin/garlicoin-qt` and `evidence/` (18 files).

Tar.gz SHA256:
`b6556a3bf0dc94da3afa3981f0b16e56ef2951b5fdaae409e19ac030310143c2`

## Validation performed

- Native depends, wallet-enabled Core and Qt GUI configure/build: PASS.
- All four binaries: `file` reports Mach-O 64-bit executable; `lipo -archs` reports x86_64.
- All four binaries: `otool -l` reports macOS deployment 10.13.
- All four binaries: `otool -L` permits only /usr/lib and /System/Library dependencies.
  No dynamic Qt, libssl/libcrypto or Berkeley DB.
- Verbose linker trace confirms Qt5Core/Gui/Widgets, libssl/libcrypto and
  libdb_cxx-4.8 archives from repository depends.
- Full `nm` includes static minimal/Cocoa plugin symbols and GUI SSL symbols.
  The linker makes Qt plugin symbols local; `nm -g` is insufficient for this check.
- SHA256 is recorded for all depends static libraries and platform plugin archives.
  Qt/OpenSSL/BDB header/package versions and Xcode/SDK environment are retained.
- `garlicoind --version`, `garlicoin-cli --version`: PASS.
- `garlicoin-tx --version` was executed and retained: this legacy utility does not
  support that option; it returns exit 1 with `error: too few parameters`.
  Its supported `-help` path prints version v0.18.2.0-b1552ca and returns success;
  `-create` also succeeds. No production TX behavior was changed.
- `QT_QPA_PLATFORM=minimal src/qt/garlicoin-qt -version`: PASS, exit 0,
  version v0.18.2.0-b1552ca (64-bit), no loader/plugin error.
- Non-blocking GUI warning retained: `Qt::AA_EnableHighDpiScaling must be set
  before QCoreApplication is created`. It is not addressed by this dependency gate.
- Downloaded artifact SHA256 and all four Mach-O headers were independently checked.

Evidence includes environment.log, archive-sha256.txt, qt-version.txt,
openssl-version.txt, bdb-version.txt, configure/core-config logs, build.log,
macho-linkage.txt, verification.log and binary version/startup output.

## Remaining validation

This is a native build and loader checkpoint, not full macOS regression validation.
Run interactive Cocoa GUI checks on real macOS: window startup, menus, dialogs, wallet operations and shutdown.
Wallet/datadir compatibility and full unit/functional regressions on macOS remain separate gates.
The 10.13 load command verifies the declared deployment floor; actual runtime compatibility on macOS 10.13 requires testing there.
DMG, signing, notarization, arm64 and universal2 are outside this checkpoint.
