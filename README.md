Garlicoin Core
==============

[![Linux CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/linux-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/linux-ci.yml)
[![Ubuntu Toolchain CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/ubuntu-toolchain-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/ubuntu-toolchain-ci.yml)
[![Qt GUI CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/qt-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/qt-ci.yml)

This repository is a maintained fork of [GarlicoinOrg/Garlicoin](https://github.com/GarlicoinOrg/Garlicoin).

The goal is not to turn Garlicoin into a different blockchain or to blindly follow the newest Litecoin Core release. The goal is to keep the existing Garlicoin Core 0.18 codebase buildable, testable, secure and maintainable on current systems while preserving compatibility with the existing Garlicoin blockchain, wallets and network.

Maintenance goal
----------------

This project treats Garlicoin Core 0.18 as a long-lived maintenance line.

Routine modernization focuses on:

- current Linux, Windows and macOS build/toolchain compatibility;
- deterministic dependency builds and reproducible release infrastructure;
- CI coverage for the daemon, CLI, wallet and Qt GUI;
- repair of inherited or stale test fixtures and test harness assumptions;
- dependency source reliability and authenticated HTTPS downloads;
- conservative dependency updates where compatibility can be demonstrated;
- robustness, security and correctness fixes that can be backported safely;
- current build and developer documentation.

Routine maintenance intentionally avoids changing Garlicoin's blockchain identity or consensus rules. In particular, the following are expected to remain compatible unless a separate, explicitly reviewed project says otherwise:

- genesis and chain parameters;
- Allium proof of work;
- DarkGravityWave difficulty adjustment;
- block rewards and emission rules;
- address prefixes and transaction/block interpretation;
- P2P network behaviour and RPC semantics where practical;
- existing Berkeley DB `wallet.dat` files and user datadirs.

Garlicoin is Litecoin-derived. For this codebase, Litecoin Core 0.18.1 is the primary upstream reference for maintenance and compatibility work. Later Litecoin releases may be consulted for individual fixes, but newer architecture or features are not imported automatically.

Current progress
----------------

The maintenance effort started by repairing Garlicoin-specific tests and establishing reliable CI before changing larger parts of the build stack. The repository has since progressed through test repair, runtime hardening, dependency cleanup and modern toolchain validation.

Completed or established work includes:

- Garlicoin-specific Base58, Bech32, key, RPC, PoW, mining, sighash and transaction test repairs;
- deterministic Linux builds with focused unit coverage;
- wallet-enabled builds with functional RPC/wallet smoke tests;
- Qt GUI + wallet build validation;
- RPC/HTTP robustness and safer remote-RPC binding behaviour;
- removal of obsolete Travis CI configuration;
- replacement of dead or unauthenticated dependency sources with verified HTTPS sources while retaining SHA-256 pinning;
- repaired legacy dependency builds for current compilers;
- libevent 2.1.11, MiniUPnPc 2.0.20180203 and ZeroMQ 4.3.1 maintenance updates aligned with the Litecoin 0.18 lineage;
- Qt 5.9.7 and related GUI dependency updates aligned with Litecoin Core 0.18.1;
- Ubuntu 22.04 as the retained baseline build environment;
- native Ubuntu 24.04 validation with GCC 13 and Python 3.12 compatibility fixes;
- CI validation of wallet, daemon, CLI and Qt paths on maintained Ubuntu hosts.

Current work
------------

The next platform stage is Windows / MinGW deterministic cross-build compatibility.

The active Windows work validates the existing pinned dependency stack with a real 64-bit MinGW-w64 cross-build and aims to produce and verify:

- `garlicoind.exe`;
- `garlicoin-cli.exe`;
- `garlicoin-qt.exe`.

As with the Linux maintenance work, Windows compatibility changes are expected to remain narrow and must not change consensus, wallet format, RPC semantics or network identity.

Planned follow-up work
----------------------

After the Windows stage, the main maintenance areas are expected to include:

- macOS toolchain and deterministic build validation;
- further modernization of the Python functional/P2P test harness;
- release/reproducible-build validation across supported platforms;
- explicit compatibility tests using existing `wallet.dat` files and datadirs;
- selective security, correctness and robustness backports from later Litecoin Core where they apply cleanly to Garlicoin 0.18;
- continued conservative dependency maintenance.

A large forward-port to a newer Core generation is not currently required for routine maintenance. The preferred direction is to keep Garlicoin Core 0.18 well-tested and maintainable for modern systems without forcing users into a new blockchain, wallet format or incompatible network transition.

Maintenance status
------------------

The `master` branch is the maintained integration branch for this fork. Changes are expected to pass the relevant GitHub Actions checks before being merged. The main Linux, Ubuntu toolchain and Qt workflows also validate matching pushes to `master`, and the README badges are scoped to that branch.

Current automated coverage includes:

- deterministic headless Linux builds with focused Garlicoin unit tests;
- wallet-enabled builds with functional smoke tests;
- Ubuntu 22.04 and Ubuntu 24.04 toolchain validation;
- deterministic Qt 5 GUI + wallet builds;
- focused dependency workflows for selected legacy or optional dependency paths;
- a manually triggered broader unit-suite diagnostic job.

Official Garlicoin release tags remain available in the [upstream repository](https://github.com/GarlicoinOrg/Garlicoin/tags). This fork should not be treated as a separate official release channel unless explicitly stated otherwise.

Building
--------

Start with [INSTALL.md](INSTALL.md). Platform-specific build instructions are available in the [`doc/`](doc/) directory, including Linux/Unix, macOS and Windows notes.

For the currently validated Linux toolchain scope, see [`doc/ubuntu-toolchain.md`](doc/ubuntu-toolchain.md).

Testing
-------

Developers are strongly encouraged to add or update tests with code changes.

Unit tests can be built and run, when enabled at configure time, with:

```sh
make check
```

Further details are in [`src/test/README.md`](src/test/README.md).

Regression and integration tests live under [`test/functional/`](test/functional/) and can be run with the functional test runner, for example:

```sh
python3 test/functional/test_runner.py
```

The CI configuration intentionally keeps pull-request validation focused and repeatable. Broader diagnostics remain available for maintenance work without making every small compatibility change depend on the heaviest possible test path.

Development process
-------------------

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md). For larger or higher-risk changes, include a clear test plan and prefer review by somebody other than the author.

For compatibility and maintenance work, keep changes narrowly scoped and document the upstream reference or reproduced failure that justifies them. Do not mix routine build/dependency maintenance with consensus or wallet-format changes.

Garlicoin developer/community discussion is available on [Discord](https://discord.gg/mmAb4ewGb6).

Translations
------------

Translation updates follow the inherited Bitcoin/Garlicoin translation process documented in [`doc/translation_process.md`](doc/translation_process.md). Avoid direct translation-only pull requests when the affected files are generated from the translation source.

License
-------

Garlicoin Core is released under the terms of the MIT license. See [COPYING](COPYING) for more information.
