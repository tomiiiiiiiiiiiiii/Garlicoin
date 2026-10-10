Unix build notes
================

These notes cover the maintained Unix/Linux build path for Garlicoin Core. For reproducible maintenance work, prefer the repository's `depends` system over manually assembling old or mismatched system libraries.

Basic build
-----------

When suitable dependencies are already available:

```sh
./autogen.sh
./configure
make
```

Run `./configure --help` for the complete option list.

Recommended deterministic build
-------------------------------

For a normal x86_64 Linux wallet + Qt build:

```sh
HOST=x86_64-unknown-linux-gnu
make -C depends HOST="$HOST"

./autogen.sh
CONFIG_SITE="$PWD/depends/$HOST/share/config.site" \
  ./configure --enable-wallet --with-gui=qt5

make
```

For a headless build without wallet, Qt, UPnP, or ZMQ:

```sh
HOST=x86_64-unknown-linux-gnu
make -C depends HOST="$HOST" NO_QT=1 NO_WALLET=1 NO_UPNP=1

./autogen.sh
CONFIG_SITE="$PWD/depends/$HOST/share/config.site" \
  ./configure \
    --disable-wallet \
    --without-gui \
    --without-miniupnpc \
    --disable-zmq

make
```

Current maintenance baseline
----------------------------

Maintained release builds currently use:

- Qt 5.15.19;
- OpenSSL 3.5.9;
- Berkeley DB 4.8.30 for wallet compatibility.

The full dependency set and exact recipes are under `depends/packages/`. See [dependencies.md](dependencies.md).

Ubuntu validation
-----------------

Ubuntu 22.04 is the release/CI compatibility baseline used by the maintained Linux and cross-build workflows. Additional modern-toolchain validation is documented in [ubuntu-toolchain.md](ubuntu-toolchain.md).

Wallet builds
-------------

Berkeley DB is required when wallet support is enabled. Existing wallet compatibility is based on Berkeley DB 4.8.

The repository also contains the historical helper:

```sh
./contrib/install_db4.sh "$PWD"
```

For release work, prefer the pinned `depends` BDB recipe so the build matches CI.

Disable the wallet with:

```sh
./configure --disable-wallet
```

GUI builds
----------

Qt 5 is the maintained GUI line for this fork.

```sh
./configure --with-gui=qt5
```

Disable the GUI with:

```sh
./configure --without-gui
```

Optional features
-----------------

UPnP can be disabled with:

```sh
./configure --without-miniupnpc
```

ZeroMQ can be disabled with:

```sh
./configure --disable-zmq
```

Build resources
---------------

C++ and Qt compilation can use substantial RAM. On constrained systems reduce parallelism before changing compiler or dependency settings, for example:

```sh
make -j1
```

Other Unix-like systems
-----------------------

The source may build on additional Unix-like systems, but the maintained public release path does not currently validate dedicated NetBSD or OpenBSD builds. Avoid treating old upstream platform instructions as supported until they are tested against the current source and dependency stack.

Source of truth
---------------

- `depends/packages/` for dependency recipes;
- `.github/workflows/linux-ci.yml` for Linux CI;
- `.github/workflows/release-validation-ci.yml` for release builds;
- `./configure --help` for current build options.
