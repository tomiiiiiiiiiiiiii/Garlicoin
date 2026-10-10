Dependencies
============

Garlicoin Core can be built against suitable system libraries or with the repository's deterministic `depends` system. For maintained CI and release builds, the recipes under `depends/packages/` are the source of truth.

Current maintenance baseline
----------------------------

The dependencies most important to the current 0.18.x maintenance line are:

| Dependency | Pinned version | Purpose |
| --- | ---: | --- |
| Qt | 5.15.19 | GUI toolkit |
| OpenSSL | 3.5.9 | Cryptographic/TLS support |
| Berkeley DB | 4.8.30.NC | Existing wallet compatibility |

The exact package set is assembled by `depends/packages/packages.mk`. Individual versions, source URLs, hashes, patches, and platform-specific configuration live in the corresponding package recipes, for example:

- `depends/packages/qt.mk`
- `depends/packages/openssl.mk`
- `depends/packages/bdb.mk`
- `depends/packages/boost.mk`
- `depends/packages/libevent.mk`
- `depends/packages/zeromq.mk`

Why this document does not duplicate every pin
----------------------------------------------

The dependency tree contains many transitive and GUI-specific packages. A copied table of every version quickly becomes stale after maintenance work. Check the package recipe when an exact version matters.

Compatibility notes
-------------------

- Berkeley DB 4.8 is intentionally retained for compatibility with existing Garlicoin wallet databases.
- Qt 5.15.19 is the maintained Qt 5 GUI baseline for this fork.
- OpenSSL 3.5.9 replaces the historical OpenSSL 1.0.1k dependency in maintained builds.
- Dependency modernization must not be assumed to permit consensus, network, address-format, or wallet-format changes.
- A successful build against an arbitrary system library version does not make that version part of the supported release baseline.

Optional package groups
-----------------------

The `depends` system conditionally adds package groups for features such as:

- Qt GUI support;
- wallet support;
- UPnP;
- ZeroMQ;
- platform-specific packaging and GUI dependencies.

See `depends/packages/packages.mk` for the current grouping rather than relying on historical Bitcoin/Litecoin dependency lists.

Building
--------

For platform-specific commands, see:

- [Unix](build-unix.md)
- [Windows x86_64](build-windows.md)
- [macOS x86_64](build-osx.md)
- [Ubuntu toolchain validation](ubuntu-toolchain.md)

For deterministic build-system details, see [`depends/README.md`](../depends/README.md).
