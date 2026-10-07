UNIX BUILD NOTES
================

These notes cover building Garlicoin Core on Unix-like systems. For OpenBSD-specific instructions, see [build-openbsd.md](build-openbsd.md).

Garlicoin Core is a legacy codebase with several intentionally old pinned dependencies. For repeatable maintenance and CI builds, prefer the repository's `depends` system over replacing system libraries with obsolete distribution packages.

Basic build
-----------

When suitable dependencies are already available:

```bash
./autogen.sh
./configure
make
# make install   # optional
```

Use `./configure --help` to see all available build options.

Recommended reproducible build with `depends`
---------------------------------------------

The `depends` directory builds the dependency versions pinned by this repository without requiring you to downgrade system libraries.

For a headless build without wallet or UPnP:

```bash
HOST=x86_64-unknown-linux-gnu
make -C depends HOST="$HOST" NO_QT=1 NO_WALLET=1 NO_UPNP=1
./autogen.sh
CONFIG_SITE="$PWD/depends/$HOST/share/config.site" \
  ./configure --disable-wallet --without-gui --without-miniupnpc --disable-zmq --enable-sse2
make
```

For a wallet-enabled headless build:

```bash
HOST=x86_64-unknown-linux-gnu
make -C depends HOST="$HOST" NO_QT=1 NO_UPNP=1
./autogen.sh
CONFIG_SITE="$PWD/depends/$HOST/share/config.site" \
  ./configure --enable-wallet --without-gui --without-miniupnpc --disable-zmq --enable-sse2
make
```

For the Qt 5 GUI with wallet support:

```bash
HOST=x86_64-unknown-linux-gnu
make -C depends HOST="$HOST" NO_UPNP=1
./autogen.sh
CONFIG_SITE="$PWD/depends/$HOST/share/config.site" \
  ./configure --enable-wallet --with-gui=qt5 --without-miniupnpc --disable-zmq --enable-sse2
make
```

The current CI compatibility baseline is Ubuntu 22.04. Newer distributions may expose compiler or dependency incompatibilities in the legacy build stack and should be validated separately before being adopted as a baseline.

Dependencies
------------

Core requirements include:

| Library | Purpose |
| --- | --- |
| OpenSSL | Cryptographic support |
| Boost | Utility, threading and test support |
| libevent | Asynchronous networking |

Optional components include:

| Library | Purpose |
| --- | --- |
| Berkeley DB | Wallet storage; 4.8 is used for compatibility |
| Qt 5 | GUI |
| protobuf | Payment protocol / GUI support |
| qrencode | QR codes in GUI |
| MiniUPnPc | Optional UPnP support |
| ZeroMQ | Optional ZMQ notifications |

For the exact versions pinned by the deterministic build system, see [dependencies.md](dependencies.md) and the recipes in [`depends/packages/`](../depends/packages/).

Ubuntu / Debian build tools
---------------------------

A typical host needs the standard C/C++ and autotools toolchain:

```bash
sudo apt-get update
sudo apt-get install -y \
  autoconf automake autotools-dev bsdmainutils build-essential \
  ca-certificates curl git libtool pkg-config python3
```

If you build against distribution libraries instead of `depends`, install the appropriate development packages for Boost, libevent, OpenSSL and any optional features you enable. Package names and available versions vary by distribution release.

Do **not** replace your distribution repositories with an obsolete Debian or Ubuntu release merely to obtain an old OpenSSL package. Use the deterministic `depends` build or a controlled build environment instead.

Wallet and Berkeley DB
----------------------

Berkeley DB is required only when wallet support is enabled. Existing Garlicoin wallet compatibility is based on Berkeley DB 4.8.

The repository includes a helper script for building the compatible version locally:

```bash
./contrib/install_db4.sh "$PWD"
```

Follow the environment/configure instructions printed by the script.

If wallet compatibility with existing Berkeley DB 4.8 builds is not required, `configure` also supports `--with-incompatible-bdb` when using another supported Berkeley DB version. Use this deliberately: wallets created or modified with incompatible database versions may not be portable to standard builds.

To build without wallet support:

```bash
./configure --disable-wallet
```

GUI
---

Qt 5 is the maintained GUI target for this fork. For reproducible builds, prefer the pinned Qt provided by `depends`.

To build without the GUI:

```bash
./configure --without-gui
```

To request Qt 5 explicitly:

```bash
./configure --with-gui=qt5
```

UPnP and ZMQ
------------

UPnP support is optional. Disable it entirely with:

```bash
./configure --without-miniupnpc
```

ZMQ notifications are also optional and can be disabled with:

```bash
./configure --disable-zmq
```

Security hardening
------------------

Hardening is enabled by default where supported. The relevant configure switches are:

```bash
./configure --enable-hardening
./configure --disable-hardening
```

Do not disable hardening merely to make an unsupported dependency combination compile; prefer fixing or isolating the compatibility issue.

Memory requirements
-------------------

C++ compilation can use substantial memory. On constrained systems, reduce parallelism first, for example:

```bash
make -j1
```

The older compiler tuning flags historically documented here should only be used when you understand their effect on your compiler version.

ARM cross-compilation
---------------------

The `depends` system supports cross-compilation. On Debian/Ubuntu hosts, for example:

```bash
sudo apt-get install g++-arm-linux-gnueabihf curl
make -C depends HOST=arm-linux-gnueabihf NO_QT=1
./autogen.sh
./configure --prefix="$PWD/depends/arm-linux-gnueabihf" \
  --enable-glibc-back-compat --enable-reduce-exports LDFLAGS=-static-libstdc++
make
```

Exact toolchain support can vary with the host distribution and should be validated in CI before publishing binaries.

FreeBSD
-------

Use GNU make (`gmake`) and install the normal autotools, Boost, OpenSSL and libevent development packages from FreeBSD ports/packages. Wallet builds additionally require a compatible Berkeley DB configuration.

Because package versions change over time, prefer current FreeBSD package names rather than relying on historical version-specific commands in this document.

Further information
-------------------

- [`depends/README.md`](../depends/README.md) describes the deterministic dependency system.
- [dependencies.md](dependencies.md) records the dependency versions pinned by this repository.
- Run `./configure --help` for the complete configure option list.
