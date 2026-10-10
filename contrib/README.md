Garlicoin Core contrib tools
============================

This directory contains auxiliary developer, build, packaging and node-operator tools. The normal Garlicoin Core compile does not use most of these files directly.

Actively used by the maintained 0.18.x build/release path
---------------------------------------------------------

- `devtools/` - developer and binary validation helpers. `symbol-check.py` and `security-check.py` are referenced by the build system.
- `macdeploy/` - required by the maintained macOS application bundle and DMG packaging path.
- `filter-lcov.py` - LCOV coverage filtering helper.
- `install_db4.sh` - helper for building Berkeley DB 4.8 for wallet compatibility.

Useful maintenance utilities
----------------------------

- `seeds/` - fixed-seed generation utilities and node lists.
- `testgen/` - generators for data-driven test vectors.
- `linearize/` - tools for producing a linearized blockchain data set.
- `qos/` - optional Linux traffic-control helper for node operators.
- `zmq/` - ZeroMQ subscriber examples used with the ZMQ interface.

Packaging material
------------------

- `debian/` - historical Debian packaging metadata and asset/copyright attribution. It is not the current GitHub Actions release path.
- `init/`, `rpm/` and the legacy bash-completion files are retained for the moment because they are still referenced by `Makefile.am` source-distribution metadata. They should only be removed together with the corresponding `Makefile.am` cleanup.

Removed legacy tooling
----------------------

The maintained release process no longer uses Gitian, the old Bitcoin binary-verification scripts, the inherited commit-signature trust set, or the obsolete Python 2 `spendfrom` utility. These were removed rather than left as misleading maintenance paths.

The supported release process is documented in `doc/release-process.md` and implemented by the repository GitHub Actions workflows.
