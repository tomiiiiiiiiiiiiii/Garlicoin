# Ubuntu toolchain validation

Ubuntu 22.04 remains the maintained Linux CI/release baseline. Ubuntu 24.04 is used as an additional native x86_64 toolchain target; it is not a new minimum runtime requirement or a Core-version port.

## Current dependency baseline

The maintained `depends` stack used by current validation includes:

- Qt 5.15.19;
- OpenSSL 3.5.9;
- Berkeley DB 4.8.30 for wallet compatibility.

Exact dependency recipes under `depends/packages/` are authoritative.

## Toolchain compatibility work

The Ubuntu 24.04 validation exposed compatibility issues that were fixed narrowly rather than by weakening compiler warnings or changing Garlicoin behavior. Examples include:

- adding the required standard-library include for newer GCC in `support/lockedpool.cpp`;
- refreshing the xcb-proto Python helper used by the pinned dependency build so it works with Python 3.12;
- porting the legacy functional-test P2P transport from removed `asyncore` APIs to `asyncio` while preserving the existing test interface.

These are build/test compatibility changes, not consensus or wallet-format changes.

## CI coverage

The maintained workflows provide complementary coverage:

- **Linux CI**: normal Linux compilation and focused tests on the baseline environment;
- **Ubuntu Toolchain CI**: native Ubuntu 22.04 and 24.04 toolchain compatibility checks;
- **Qt GUI CI**: explicit Qt GUI configuration and link validation;
- **Release Validation CI**: release-oriented Linux, Windows, and native macOS builds and packaging checks.

Check the current workflow definitions under `.github/workflows/` for the exact job matrix and commands. The workflow YAML is the source of truth when an older validation note and CI differ.

## Scope and limits

Ubuntu toolchain validation does not by itself prove:

- bit-for-bit reproducibility across distributions;
- support for arbitrary system dependency versions;
- runtime compatibility with every historical Linux distribution;
- Windows or macOS behavior outside their dedicated workflows.

The maintenance scope preserves Garlicoin consensus, Allium proof of work, network identity, chain parameters, address formats, serialization, existing datadir compatibility, and the Berkeley DB 4.8 wallet format unless a separate explicitly reviewed change says otherwise.

## Related documentation

- [Unix build notes](build-unix.md)
- [Dependencies](dependencies.md)
- [Release process](release-process.md)
