Garlicoin Core
==============

[![Linux CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/linux-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/linux-ci.yml)
[![Qt GUI CI](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/qt-ci.yml/badge.svg?branch=master)](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/workflows/qt-ci.yml)

This repository is a maintained fork of [GarlicoinOrg/Garlicoin](https://github.com/GarlicoinOrg/Garlicoin). It carries conservative build, test, compatibility, and robustness fixes while keeping consensus and network behaviour changes out of routine maintenance.

Maintenance status
------------------

The `master` branch is the maintained integration branch for this fork. Changes are expected to pass the relevant GitHub Actions checks before being merged. The main Linux and Qt workflows also validate matching pushes to `master`, and the README badges are scoped to that branch.

Current automated coverage includes:

- a deterministic headless Linux build with focused Garlicoin unit tests;
- a wallet-enabled build with functional smoke tests;
- a manually triggered full unit-suite diagnostic job;
- a deterministic Qt 5 GUI + wallet build.

Official Garlicoin release tags remain available in the [upstream repository](https://github.com/GarlicoinOrg/Garlicoin/tags). This fork should not be treated as a separate official release channel unless explicitly stated otherwise.

Building
--------

Start with [INSTALL.md](INSTALL.md). Platform-specific build instructions are available in the [`doc/`](doc/) directory, including Linux/Unix, macOS, and Windows notes.

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

The CI configuration intentionally keeps the pull-request signal focused and repeatable. The broader unit suite is also available as a manually triggered diagnostic workflow for maintenance work.

Development process
-------------------

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md). For larger or higher-risk changes, include a clear test plan and prefer review by somebody other than the author.

Garlicoin developer/community discussion is available on [Discord](https://discord.gg/mmAb4ewGb6).

Translations
------------

Translation updates follow the inherited Bitcoin/Garlicoin translation process documented in [`doc/translation_process.md`](doc/translation_process.md). Avoid direct translation-only pull requests when the affected files are generated from the translation source.

License
-------

Garlicoin Core is released under the terms of the MIT license. See [COPYING](COPYING) for more information.
