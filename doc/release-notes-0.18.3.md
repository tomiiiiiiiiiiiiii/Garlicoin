# Garlicoin Core 0.18.3

Garlicoin Core 0.18.3 is a maintenance release focused on dependency modernization, build reliability, and current-platform compatibility while preserving the existing Garlicoin network, blockchain, wallet format, RPC interface, and Berkeley DB 4.8 wallet compatibility.

## Highlights

- Updated the Qt 5 line to Qt 5.15.19.
- Updated OpenSSL to 3.5.9.
- Preserved Berkeley DB 4.8.30 wallet compatibility.
- Added and validated current Linux, Windows x86_64, and native macOS x86_64 build paths.
- Improved Python 3.12+ compatibility in the P2P functional test framework.
- Removed obsolete BIP70 payment-protocol support while retaining ordinary `garlicoin:` URI handling.
- Added maintenance and release-validation CI coverage for current toolchains.

## Compatibility

This release does not intentionally change consensus rules, proof of work, network parameters, address formats, the existing blockchain/datadir layout, the wallet format, or normal RPC behavior.

Existing `wallet.dat` files remain on Berkeley DB 4.8 compatibility.

## Release platforms

The release workflow validates and publishes:

- Linux x86_64 archive
- Windows x86_64 archive
- Windows x86_64 installer (`garlicoin-0.18.3-win64-setup.exe`)
- macOS x86_64 archive
- macOS x86_64 DMG
- source archive
- SHA256 checksums

## Dependency baseline

- Qt 5.15.19
- OpenSSL 3.5.9
- Berkeley DB 4.8.30

## macOS

The supported release build is produced natively on an Intel macOS runner with Xcode 16 / macOS SDK 15.0 and a deployment target of macOS 10.13. Qt, OpenSSL, and Berkeley DB are linked from the repository `depends` build; non-system dynamic Qt/OpenSSL/BDB dependencies are not expected.

The deployment target is validated from the Mach-O load commands. Actual execution on every historical macOS point release is not implied by that metadata alone.

## Windows

Windows release binaries are built for x86_64 with MinGW and packaged both as a ZIP archive and an NSIS installer. Qt and OpenSSL are linked statically in the validated release build.

## Notes

This is a maintenance release in the Garlicoin Core 0.18.x line. A larger UI refresh or Qt 6 migration is intentionally outside the scope of 0.18.3.
