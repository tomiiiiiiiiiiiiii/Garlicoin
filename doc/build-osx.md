macOS BUILD NOTES
=================

This document describes the currently verified native macOS x86_64 build path for the maintained Garlicoin Core 0.18.x line.

Validated CI baseline
---------------------

The repository currently validates macOS with:

- Intel x86_64 runner: `macos-15-intel`;
- Xcode 16;
- macOS SDK 15.0;
- deployment target: macOS 10.13;
- Qt 5.15.19;
- OpenSSL 3.5.9;
- Berkeley DB 4.8.30 for wallet compatibility.

Qt, OpenSSL, Berkeley DB, and the other pinned dependencies are built through the repository `depends` system. The release build verifies that Qt/OpenSSL/BDB are not left as non-system dynamic dependencies.

The CI baseline is the supported release path. Other macOS/Xcode combinations may work, but are not implied to be validated by this document.

Prerequisites
-------------

Install Xcode 16 and select it as the active developer directory. Install the build tools with Homebrew:

```sh
brew install autoconf automake libtool pkg-config make
```

Verify the native architecture, Xcode, and SDK:

```sh
uname -m
xcodebuild -version
xcrun --sdk macosx --show-sdk-version
```

The validated CI values are `x86_64`, Xcode 16, and SDK 15.0.

Build pinned dependencies
-------------------------

From the repository root:

```sh
export MACOSX_DEPLOYMENT_TARGET=10.13
export DEVELOPER_DIR=/Applications/Xcode_16.app/Contents/Developer
export SDKROOT="$(xcrun --sdk macosx --show-sdk-path)"
export OSX_SDK_VERSION="$(xcrun --sdk macosx --show-sdk-version)"
export HOST="$(depends/config.guess)"

gmake -j3 -C depends HOST="$HOST" \
  OSX_MIN_VERSION="$MACOSX_DEPLOYMENT_TARGET" \
  OSX_SDK_VERSION="$OSX_SDK_VERSION" \
  OSX_SDK="$SDKROOT" \
  darwin_CXX="$(xcrun -f clang++) -mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET -stdlib=libc++ -Wno-error=enum-constexpr-conversion" \
  HOST_ID_SALT=native-xcode16-enum-compat \
  darwin_native_packages= \
  qt_config_opts_darwin="-platform macx-clang -no-framework"
```

The detected host must be an `x86_64-apple-darwin*` target for the validated release configuration.

Configure and build Core + Qt
-----------------------------

```sh
./autogen.sh
CONFIG_SITE="$PWD/depends/$HOST/share/config.site" \
  ./configure \
    --enable-wallet \
    --with-gui=qt5 \
    --disable-tests \
    --disable-bench

gmake -j3
```

Expected binaries include:

```text
src/garlicoind
src/garlicoin-cli
src/garlicoin-tx
src/qt/garlicoin-qt
```

Create the app bundle
---------------------

The native release path creates the application bundle with:

```sh
gmake appbundle
```

The resulting GUI executable is located at:

```text
Garlicoin-Qt.app/Contents/MacOS/Garlicoin-Qt
```

The release workflow additionally creates and validates the DMG using native macOS tooling.

Verification
------------

For release work, do not rely only on a successful compile. The repository's native macOS and release-validation workflows check the Mach-O architecture, deployment target, static dependency linkage, CLI/GUI version output, app bundle, and release packaging.

See:

- `.github/workflows/macos-native-qt515.yml`
- `.github/workflows/release-validation-ci.yml`

Legacy Linux-to-Darwin cross-build notes are intentionally not part of this guide. That path is retained only as a manual historical workflow and is not the supported Qt 5.15.19 macOS release path.
