Windows x86_64 build notes
==========================

This document describes the maintained 64-bit Windows cross-build used by Garlicoin Core CI and release validation.

Validated target
----------------

- build host: Ubuntu 22.04;
- target: `x86_64-w64-mingw32`;
- MinGW-w64 POSIX thread model;
- deterministic `depends` stack;
- wallet enabled;
- Qt 5 GUI enabled.

The current dependency baseline includes Qt 5.15.19, OpenSSL 3.5.9, and Berkeley DB 4.8.30 for wallet compatibility.

Host tools
----------

Install the normal autotools/build utilities and the x86_64 MinGW-w64 compiler. The repository CI workflow is the source of truth for the exact package list used by automation.

Select the POSIX MinGW variants:

```sh
sudo update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix
sudo update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix
```

Build
-----

From the repository root:

```sh
HOST=x86_64-w64-mingw32
make -j2 -C depends HOST="$HOST"

./autogen.sh
CONFIG_SITE="$PWD/depends/$HOST/share/config.site" \
  ./configure \
    --enable-wallet \
    --with-gui=qt5 \
    --disable-tests \
    --disable-bench

make -j2 -C src
```

Expected executables include:

```text
src/garlicoind.exe
src/garlicoin-cli.exe
src/garlicoin-tx.exe
src/qt/garlicoin-qt.exe
```

Verification
------------

Confirm the binaries are Windows x86_64 PE files rather than host Linux executables:

```sh
file src/garlicoind.exe
file src/garlicoin-cli.exe
file src/garlicoin-tx.exe
file src/qt/garlicoin-qt.exe
```

The result should identify `PE32+` / `x86-64` Windows executables.

For another target-format check:

```sh
x86_64-w64-mingw32-objdump -f src/qt/garlicoin-qt.exe
```

The reported format should be `pei-x86-64`.

Release builds
--------------

A successful compile is not sufficient for a public release. The repository's release-validation workflow builds and packages the maintained Windows artifacts and performs target/version checks before publication.

Use the pinned `depends` stack for release reproduction. Do not silently substitute host Qt, OpenSSL, Berkeley DB, or other libraries and then treat the result as equivalent to the validated release build.

Scope
-----

This document does not claim CI validation for native Visual Studio builds, 32-bit Windows, Cygwin, or arbitrary MSYS2 configurations. Those may be useful development environments but are outside the maintained release path until explicitly validated.

Source of truth
---------------

- `.github/workflows/windows-ci.yml` for normal Windows CI;
- `.github/workflows/release-validation-ci.yml` for release validation;
- `depends/packages/` for pinned dependency versions and build recipes.
