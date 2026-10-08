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
| Expat | 2.2.6 | XML support in GUI dependencies |
| libevent | 2.1.11-stable | Networking |
| MiniUPnPc | 2.0.20180203 | Optional UPnP support |
| OpenSSL | 1.0.1k | Cryptographic support used by this legacy codebase |
| protobuf | 2.6.1 | Payment protocol / GUI support |
| qrencode | 3.4.4 | Optional QR code support |
| Qt | 5.9.7 | GUI toolkit |
| ZeroMQ | 4.3.1 | Optional ZMQ notifications |
| zlib | 1.2.11 | Compression support used by the dependency stack |

The authoritative values are the package recipes under [`depends/packages/`](../depends/packages/). Update this document when those recipes change.

Security and compatibility note
-------------------------------

Several pinned dependencies are intentionally old because Garlicoin Core inherits a legacy build and compatibility stack. A version being pinned here does **not** mean it is current or free of known vulnerabilities.

Do not infer security status from this table. Dependency upgrades should be reviewed and tested individually because they can affect wallet compatibility, deterministic builds, GUI compatibility, compiler support, or runtime behaviour.

In particular:

- Berkeley DB 4.8 is retained for wallet compatibility with existing builds;
- Qt 5.9.7 and OpenSSL 1.0.1k are legacy pins and should not be treated as modern security baselines;
- routine maintenance should avoid changing consensus or network behaviour while updating build infrastructure.

The Qt 5.9.7 recipe, source hashes, static-plugin configure checks and base
patch set follow [Litecoin Core v0.18.1](https://github.com/litecoin-project/litecoin/tree/v0.18.1).
Garlicoin uses Qt's HTTPS archive location and additionally includes the missing
`<limits>` header needed to compile Qt 5.9.7 with newer GCC versions. Expat,
FreeType build options and libxcb configuration support are aligned with the
same Litecoin release. This is build/GUI maintenance; Berkeley DB 4.8 and
OpenSSL 1.0.1k remain unchanged.

System dependencies
-------------------

When building against system libraries instead of `depends`, the exact package versions vary by operating system. See the relevant `build-*.md` document for platform-specific package names and configure options.

For deterministic builds, start with [`depends/README.md`](../depends/README.md).
