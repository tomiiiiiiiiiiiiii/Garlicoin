Garlicoin Core 0.18.4 release notes
===================================

Garlicoin Core 0.18.4 is a maintenance release for the actively maintained 0.18.x line.

Highlights
----------

- Restores the original Garlicoin wallet appearance after the experimental 0.18.3 GUI refresh.
- Initializes Qt High-DPI application attributes before application construction so Windows display scaling can resize text and controls consistently.
- Keeps the current GRLC.eu maintenance branding and release metadata.
- Includes release-workflow and packaging verification fixes used for the final 0.18.4 artifacts.

Compatibility
-------------

This release does not intentionally change Garlicoin consensus, proof of work, network identity, address formats, existing blockchain/datadir compatibility, or the Berkeley DB wallet format.

Existing Garlicoin Core 0.18.x datadirs and wallets are intended to remain compatible.

Dependency baseline
-------------------

The maintained release baseline remains:

- Qt 5.15.19;
- OpenSSL 3.5.9;
- Berkeley DB 4.8.30 wallet compatibility.

Validated release platforms
---------------------------

The published x86_64 release set is validated for:

- Linux;
- Windows via MinGW;
- native macOS x86_64.

Release artifacts
-----------------

Final binaries, source archive, and SHA256 checksums are published with the GitHub release:

https://github.com/tomiiiiiiiiiiiiii/Garlicoin/releases/tag/v0.18.4

Verify downloaded artifacts against the published `SHA256SUMS` file before installation.
