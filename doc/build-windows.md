WINDOWS BUILD NOTES
===================

This document describes the currently verified 64-bit Windows cross-build path for Garlicoin Core.

The supported maintenance target is:

- build host: Ubuntu 22.04;
- target: `x86_64-w64-mingw32`;
- MinGW-w64 POSIX thread model;
- deterministic `depends` build;
- wallet enabled;
- Qt 5 GUI enabled.

This procedure is validated by the repository's Windows MinGW CI. It builds and verifies:

- `src/garlicoind.exe`;
- `src/garlicoin-cli.exe`;
- `src/qt/garlicoin-qt.exe`.

The CI additionally checks that these are 64-bit Windows PE executables rather than host Linux ELF binaries.

Garlicoin is Litecoin-derived. Litecoin Core 0.18.1 remains the primary upstream reference for this build generation, but the commands below reflect the procedure actually validated for the current Garlicoin tree on Ubuntu 22.04.

64-bit Windows cross-build on Ubuntu 22.04
------------------------------------------

Install the host build tools and the 64-bit MinGW-w64 compiler:

    sudo apt-get update
    sudo apt-get install -y --no-install-recommends \
        autoconf \
        automake \
        autotools-dev \
        bsdmainutils \
        build-essential \
        ca-certificates \
        ccache \
        curl \
        file \
        g++-mingw-w64-x86-64 \
        git \
        gperf \
        libtool \
        mingw-w64-tools \
        patch \
        perl \
        pkg-config \
        python3

Select the POSIX MinGW thread model for both C and C++:

    sudo update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix
    sudo update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix

The POSIX variant is required by this C++11-era codebase. The Win32 thread variant conflicts with standard threading facilities used by the source tree.

You can confirm the selected compiler with:

    x86_64-w64-mingw32-gcc --version
    x86_64-w64-mingw32-g++ --version
    x86_64-w64-mingw32-g++ -v

Build the pinned deterministic dependencies:

    make -j2 -C depends HOST=x86_64-w64-mingw32

Then configure Garlicoin Core using the generated depends configuration:

    ./autogen.sh
    CONFIG_SITE="$PWD/depends/x86_64-w64-mingw32/share/config.site" \
        ./configure \
            --enable-wallet \
            --with-gui=qt5 \
            --disable-tests \
            --disable-bench

Build the Windows executables:

    make -j2 -C src

Expected binaries
-----------------

A successful wallet + Qt build produces at least:

    src/garlicoind.exe
    src/garlicoin-cli.exe
    src/qt/garlicoin-qt.exe

Verify that the files are 64-bit Windows PE executables:

    file src/garlicoind.exe
    file src/garlicoin-cli.exe
    file src/qt/garlicoin-qt.exe

Each result should identify a `PE32+ executable`, `x86-64`, for Microsoft Windows.

For an additional target-format check:

    x86_64-w64-mingw32-objdump -f src/garlicoind.exe
    x86_64-w64-mingw32-objdump -f src/garlicoin-cli.exe
    x86_64-w64-mingw32-objdump -f src/qt/garlicoin-qt.exe

The reported file format should be:

    pei-x86-64

Depends system
--------------

The Windows cross-build uses the versions pinned by the Garlicoin `depends` system. Do not replace those dependencies with host libraries when reproducing the deterministic build.

See [depends/README.md](../depends/README.md) for additional information about the depends framework.

Current compatibility scope
---------------------------

The verified path above is specifically the 64-bit MinGW-w64 cross-build on Ubuntu 22.04.

The following are not claimed as currently CI-validated by this document:

- native Visual Studio builds;
- Cygwin or MSYS2 builds;
- 32-bit Windows builds;
- WSL-specific filesystem/install procedures;
- runtime Windows testing under Wine or on a Windows runner.

These environments may work, but should not be documented as supported until they are tested against the current Garlicoin tree.

Dependency compatibility
------------------------

The current verified Windows build retains the existing pinned compatibility baseline, including:

- Berkeley DB 4.8 for wallet compatibility;
- Qt 5.9.7;
- OpenSSL 1.0.1k.

A successful modern MinGW cross-build does not by itself justify changing these versions. Dependency upgrades should remain separate, evidence-driven maintenance work.
