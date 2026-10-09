Garlicoin Core
==============

[![Linux CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/linux-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/linux-ci.yml)
[![Ubuntu Toolchain CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/ubuntu-toolchain-ci.yml/badge.svg?event=pull_request)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/ubuntu-toolchain-ci.yml)
[![Qt GUI CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/qt-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/qt-ci.yml)
[![Windows MinGW CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/windows-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/windows-ci.yml)
[![macOS Deterministic CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/macos-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/macos-ci.yml)
[![Release Validation CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/release-validation-ci.yml/badge.svg?event=pull_request)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/release-validation-ci.yml)
![Maintained 2026](https://img.shields.io/badge/maintained-2026-brightgreen)
![Version 0.18.2](https://img.shields.io/badge/version-0.18.2-blue)
![License MIT](https://img.shields.io/badge/license-MIT-blue)

This repository is a maintained fork of [GarlicoinOrg/Garlicoin](https://github.com/GarlicoinOrg/Garlicoin), focused on keeping Garlicoin Core **0.18.2** buildable, testable and maintainable on current systems.

The maintenance line preserves Garlicoin consensus, Allium proof of work, DarkGravityWave, wallet format, address formats, network identity and existing user datadirs. Garlicoin is Litecoin-derived, with Litecoin Core 0.18.1 used as the primary upstream reference for compatible maintenance fixes.

Maintained builds are identified as the **grlc.eu edition (2026)** to distinguish the actively maintained binaries from older upstream builds.

Latest release
--------------

The current maintained release is **Garlicoin Core 0.18.2 - GRLC.eu Maintenance Release**.

[View release notes and all downloads](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/releases/tag/v0.18.2)

Downloads:

- [Windows installer (x86_64)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/releases/download/v0.18.2/garlicoin-0.18.2-win64-setup.exe)
- [Windows ZIP (x86_64)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/releases/download/v0.18.2/garlicoin-0.18.2-win64.zip)
- [Linux tarball (x86_64)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/releases/download/v0.18.2/garlicoin-0.18.2-linux-x86_64.tar.gz)
- [macOS DMG (x86_64)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/releases/download/v0.18.2/garlicoin-0.18.2-osx64.dmg)
- [macOS tarball (x86_64)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/releases/download/v0.18.2/garlicoin-0.18.2-osx64.tar.gz)
- [Source tarball](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/releases/download/v0.18.2/garlicoin-0.18.2.tar.gz)
- [SHA256SUMS](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/releases/download/v0.18.2/SHA256SUMS)

The release binaries were produced by the cross-platform Release Validation CI for the `v0.18.2` tag. Verify downloaded files against `SHA256SUMS` before installation.

Current status
--------------

Validated and maintained:

- Linux builds with wallet, Qt GUI and functional smoke coverage;
- Ubuntu 22.04 and 24.04 toolchain compatibility;
- Windows x86_64 MinGW deterministic cross-builds;
- macOS x86_64 deterministic cross-builds;
- Qt 5.9.7 GUI + wallet validation;
- cross-platform release validation for Linux, Windows and macOS;
- Linux binary/source tarballs, Windows ZIP + NSIS installer, and macOS tarball + DMG;
- SHA256 manifests for validated release artifacts;
- Python 3.12+ compatible P2P functional-test transport based on the Litecoin Core 0.18.1 lineage;
- legacy BIP70 Payment Protocol runtime and implementation removed while normal `garlicoin:` payment URIs remain supported;
- maintained dependency sources and selected dependency/security fixes;
- Garlicoin branding cleanup in GUI, translations and runtime-facing messages.

Building
--------

Start with [INSTALL.md](INSTALL.md).

Platform notes:

- Linux / Ubuntu: [`doc/ubuntu-toolchain.md`](doc/ubuntu-toolchain.md)
- Windows: [`doc/build-windows.md`](doc/build-windows.md)
- macOS: [`doc/build-osx.md`](doc/build-osx.md)

Testing
-------

```sh
make check
python3 test/functional/test_runner.py
```

See [`src/test/README.md`](src/test/README.md) and [`test/functional/`](test/functional/) for details.

Development
-----------

The `master` branch is the maintained integration branch. Maintenance changes should remain narrowly scoped and should not mix routine build/dependency work with consensus or wallet-format changes.

See [CONTRIBUTING.md](CONTRIBUTING.md).

Official Garlicoin release tags remain available in the [upstream repository](https://github.com/GarlicoinOrg/Garlicoin/tags).

Community discussion: [Discord](https://discord.gg/mmAb4ewGb6)

License
-------

Garlicoin Core is released under the MIT license. See [COPYING](COPYING).
