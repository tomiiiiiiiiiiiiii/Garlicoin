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

Compatibility placeholders
--------------------------

`Makefile.am` still includes the legacy paths `contrib/init`, `contrib/rpm` and three historically named bash-completion files in source-distribution metadata. To avoid changing the build system during this cleanup:

- `init/` and `rpm/` now contain only short notes explaining that the old upstream templates were removed;
- `bitcoin-cli.bash-completion`, `bitcoin-tx.bash-completion` and `bitcoind.bash-completion` retain their historical filenames but now register completion for `garlicoin-cli`, `garlicoin-tx` and `garlicoind` respectively.

`debian/` now retains only its README and the copyright/asset attribution record; the obsolete Bitcoin package recipes were removed.

Removed legacy tooling
----------------------

The maintained release process no longer uses Gitian, the old Bitcoin binary-verification scripts, the inherited commit-signature trust set, the obsolete qmake form project, or the Python 2 `spendfrom` utility. These were removed rather than left as misleading maintenance paths.

The supported release process is documented in `doc/release-process.md` and implemented by the repository GitHub Actions workflows.
