# Qt 5.15.19 / OpenSSL 3.5.9 macOS stage 3 blocker

Date: 2026-10-09.
Inspected branch: maintenance/qt515-openssl35.
Starting head: 591f22fd6c673937058bf898b05afec2f63b30bb.

## Scope and result

The user authorized macOS only, x86_64, with minimal toolchain/SDK/deployment
adaptation, and explicitly required stopping if a wider toolchain/code migration
was necessary. Linux and Windows builds/tests were not repeated.

Status: STOPPED AT SOURCE PREFLIGHT. No macOS dependency, Core or GUI build was
started, no Mach-O/linkage validation was performed, and no ready-binary artifact
was produced. This is not a compilation failure log or a successful platform proof.

## Verified prerequisite mismatch

| Setting | Current Darwin cross-build | Qt 5.15.19 source |
| --- | --- | --- |
| Compiler | bundled upstream Clang 3.7.1 | newer compiler needed for availability builtins |
| SDK | MacOSX10.11.sdk | QT_MAC_SDK_VERSION_MIN = 10.14 |
| Deployment | 10.8 | QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13 |
| Linker | ld64 253.9, old pinned cctools-port | compatibility with replacement compiler/SDK not validated |

Repository evidence:
- depends/hosts/darwin.mk pins SDK 10.11, deployment 10.8, ld64 253.9.
- depends/packages/native_cctools.mk pins cctools-port
  807d6fd1be5d2224872e381870c0a75387fe05e6 and upstream Clang 3.7.1.
  The recipe stages that Clang, its resource headers and libLTO.so as the target
  toolchain.
- .github/workflows/macos-ci.yml installs a host Clang and temporarily uses it to
  compile cctools. This does not replace the staged Clang 3.7.1 used for target
  builds. The workflow still downloads SDK 10.11.
- depends/patches/qt/mac-qmake.conf overrides Qt deployment with MAC_MIN_VERSION.

Exact upstream Qt source inspected at v5.15.19-lts-lgpl:
- mkspecs/common/macx.conf sets deployment 10.13 and SDK minimum 10.14.
- src/corelib/kernel/qcore_mac.mm, qt_mac_applicationIsInDarkMode(), uses
  __builtin_available(macOS 10.14, *) when building with SDK >= 10.14.
- LLVM documents availability-check syntax as introduced with LLVM 5.0.
  Consequently the pinned upstream Clang 3.7.1 cannot compile this path once the
  SDK is raised to the Qt minimum. Raising just SDK/deployment variables cannot
  complete the migration.

Sources:
- https://github.com/qt/qtbase/blob/v5.15.19-lts-lgpl/mkspecs/common/macx.conf
- https://github.com/qt/qtbase/blob/v5.15.19-lts-lgpl/src/corelib/kernel/qcore_mac.mm
- https://clang.llvm.org/docs/LanguageExtensions.html#objective-c-available

## Stop boundary

Completing this cross-build requires replacing the bundled target compiler,
updating its download/hash/resource-header/staging setup, and validating the new
compiler and SDK against the pinned cctools/ld64. A linker replacement might also
be required; that has not been demonstrated by a build and is not asserted here.

This is beyond a small SDK/deployment edit to the existing working Darwin stack.
No speculative compiler/linker version or Qt-source compatibility workaround was
committed. OpenSSL-specific Darwin compatibility remains untested, not classified
as broken.

The current executor is Linux and has no Clang, Darwin tools, Apple SDK or xcrun.
The available GitHub connector exposes no workflow_dispatch operation. No CI was
dispatched, no PR was opened, and no Linux/Windows matrix was triggered.

## Separately scoped next gate

Authorize a Darwin cross-toolchain modernization first: select a pinned compiler
supporting Qt's availability syntax, select and verify SDK >=10.14, preserve
x86_64 and use deployment 10.13 consistently for dependencies/Core/GUI. Review the
OS support-floor change explicitly. Validate ld64/cctools with the chosen SDK
before building Qt. An alternative native macOS/Xcode build changes the existing
build route and should be explicitly chosen rather than silently substituted.

Then build only Darwin depends, Core and GUI, inspect all produced Mach-O files
and load commands, verify static Qt/OpenSSL provenance with link maps and archive
hashes, reject unintended host-library dependencies, and upload binaries plus
verification evidence. No consensus, PoW, wallet format, BDB, P2P or RPC changes
are part of this prerequisite.
