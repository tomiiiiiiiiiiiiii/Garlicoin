Garlicoin Core
==============

[![Linux CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/linux-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/linux-ci.yml)
[![Ubuntu Toolchain CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/ubuntu-toolchain-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/ubuntu-toolchain-ci.yml)
[![Qt GUI CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/qt-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/qt-ci.yml)
[![Windows MinGW CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/windows-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/windows-ci.yml)
[![macOS Deterministic CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/macos-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/macos-ci.yml)
[![Release Validation CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/release-validation-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/release-validation-ci.yml)
![Maintained 2026](https://img.shields.io/badge/maintained-2026-brightgreen)
![Version 0.18.1](https://img.shields.io/badge/version-0.18.1-blue)
![License MIT](https://img.shields.io/badge/license-MIT-blue)

This repository is a maintained fork of [GarlicoinOrg/Garlicoin](https://github.com/GarlicoinOrg/Garlicoin).

The goal is to keep the Garlicoin Core 0.18.1 maintenance line buildable, testable and maintainable on current systems without changing the Garlicoin blockchain, wallet format or network identity.

Maintenance direction
---------------------

Garlicoin Core 0.18.1 is treated here as a long-lived maintenance line.

Work focuses on:

- current Linux, Windows and macOS toolchain compatibility;
- deterministic dependency builds and reproducible release validation;
- CI coverage for daemon, CLI, wallet and Qt GUI;
- repair of stale inherited tests and test harness assumptions;
- secure and reliable dependency sources;
- conservative dependency updates and selected robustness/security backports.

Routine maintenance intentionally preserves:

- genesis and chain parameters;
- Allium proof of work;
- DarkGravityWave difficulty adjustment;
- rewards and emission rules;
- address prefixes and transaction/block interpretation;
- existing Berkeley DB `wallet.dat` files and user datadirs;
- network and RPC compatibility where practical.

Garlicoin is Litecoin-derived. Litecoin Core 0.18.1 is the primary upstream reference for this maintenance line. Later Litecoin releases may be consulted for individual fixes, but newer architecture or features are not imported automatically.

Current status
--------------

Completed or established:

- repaired Garlicoin-specific test fixtures and mining/PoW test harnesses;
- deterministic Linux builds with wallet and functional smoke coverage;
- Qt 5.9.7 GUI + wallet validation;
- RPC/HTTP hardening;
- replacement of dead or unauthenticated dependency sources with verified HTTPS sources;
- maintenance updates for libevent, MiniUPnPc and ZeroMQ;
- Ubuntu 22.04 and Ubuntu 24.04 validation, including GCC 13 and Python 3.12 compatibility fixes;
- Windows / MinGW 64-bit deterministic cross-build validation for `garlicoind.exe`, `garlicoin-cli.exe` and `garlicoin-qt.exe`;
- deterministic macOS x86_64 cross-build validation for `garlicoind`, `garlicoin-cli` and `garlicoin-qt`;
- cross-platform release validation for Linux, Windows and macOS;
- Linux binary and source tarball validation;
- Windows ZIP and NSIS installer validation;
- macOS binary tarball and DMG validation;
- SHA256 manifests and CI artifact upload for validated release outputs;
- Garlicoin branding cleanup across Qt translations and runtime-facing messages;
- maintained application version set to `0.18.1`.

Planned follow-up work includes further Python/P2P test-harness cleanup, explicit compatibility tests with existing wallets and datadirs, and additional conservative maintenance backports where justified.

The `master` branch is the maintained integration branch. Relevant GitHub Actions checks are expected to pass before maintenance changes are merged.

Official Garlicoin release tags remain available in the [upstream repository](https://github.com/GarlicoinOrg/Garlicoin/tags). This fork should not be treated as a separate official release channel unless explicitly stated otherwise.

Building
--------

Start with [INSTALL.md](INSTALL.md). Platform-specific instructions are available in [`doc/`](doc/).

For the currently validated Linux toolchain scope, see [`doc/ubuntu-toolchain.md`](doc/ubuntu-toolchain.md). The validated Windows cross-build procedure is documented in [`doc/build-windows.md`](doc/build-windows.md), and the macOS build procedure is documented in [`doc/build-osx.md`](doc/build-osx.md).

Testing
-------

Unit tests:

```sh
make check
```

Functional tests:

```sh
python3 test/functional/test_runner.py
```

Further details are available in [`src/test/README.md`](src/test/README.md) and [`test/functional/`](test/functional/).

Development process
-------------------

See [CONTRIBUTING.md](CONTRIBUTING.md).

Maintenance changes should stay narrowly scoped and document the upstream reference or reproduced failure that justifies them. Routine build/dependency maintenance should not be mixed with consensus or wallet-format changes.

Garlicoin developer/community discussion is available on [Discord](https://discord.gg/mmAb4ewGb6).

License
-------

Garlicoin Core is released under the terms of the MIT license. See [COPYING](COPYING) for more information.
