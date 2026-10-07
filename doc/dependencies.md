Dependencies
============

Garlicoin Core can be built either against suitable system libraries or with the repository's deterministic `depends` system. For reproducible maintenance and CI builds, `depends` is the reference for the versions actually pinned by this repository.

Pinned `depends` versions
-------------------------

The main packages currently pinned by `depends/packages/` are:

| Dependency | Pinned version | Purpose |
| --- | ---: | --- |
| Berkeley DB | 4.8.30.NC | Wallet database compatibility |
| Boost | 1.70.0 | Utility, threading and test support |
| libevent | 2.1.8-stable | Networking |
| MiniUPnPc | 2.0.20170509 | Optional UPnP support |
| OpenSSL | 1.0.1k | Cryptographic support used by this legacy codebase |
| protobuf | 2.6.1 | Payment protocol / GUI support |
| qrencode | 3.4.4 | Optional QR code support |
| Qt | 5.7.1 | GUI toolkit |
| ZeroMQ | 4.2.2 | Optional ZMQ notifications |
| zlib | 1.2.11 | Compression support used by the dependency stack |

The authoritative values are the package recipes under [`depends/packages/`](../depends/packages/). Update this document when those recipes change.

Security and compatibility note
-------------------------------

Several pinned dependencies are intentionally old because Garlicoin Core inherits a legacy build and compatibility stack. A version being pinned here does **not** mean it is current or free of known vulnerabilities.

Do not infer security status from this table. Dependency upgrades should be reviewed and tested individually because they can affect wallet compatibility, deterministic builds, GUI compatibility, compiler support, or runtime behaviour.

In particular:

- Berkeley DB 4.8 is retained for wallet compatibility with existing builds;
- Qt 5.7.1 and OpenSSL 1.0.1k are legacy pins and should not be treated as modern security baselines;
- routine maintenance should avoid changing consensus or network behaviour while updating build infrastructure.

System dependencies
-------------------

When building against system libraries instead of `depends`, the exact package versions vary by operating system. See the relevant `build-*.md` document for platform-specific package names and configure options.

For deterministic builds, start with [`depends/README.md`](../depends/README.md).
