Garlicoin Core documentation
============================

This directory contains documentation relevant to the maintained Garlicoin Core 0.18.x line.

The repository source, `depends` recipes, and GitHub Actions workflows are the source of truth when a document and the implementation disagree. The currently validated release platforms are Linux x86_64, Windows x86_64 (MinGW), and native macOS x86_64.

Building
--------

- [Dependencies](dependencies.md)
- [Unix build notes](build-unix.md)
- [Ubuntu toolchain validation](ubuntu-toolchain.md)
- [Windows x86_64 build notes](build-windows.md)
- [macOS x86_64 build notes](build-osx.md)

Development
-----------

- [Developer notes](developer-notes.md)
- [Release process](release-process.md)
- [Unauthenticated REST interface](REST-interface.md)
- [Shared libraries](shared-libraries.md)
- [BIPs](bips.md)
- [DNS seed policy](dnsseed-policy.md)
- [Benchmarking](benchmarking.md)
- [Fuzz testing](fuzzing.md)
- [Translation strings policy](translation_strings_policy.md)

Operation and integration
-------------------------

- [Data files](files.md)
- [Reduce traffic](reduce-traffic.md)
- [Tor support](tor.md)
- [Init scripts](init.md)
- [ZMQ](zmq.md)

Release notes
-------------

- [Garlicoin Core 0.18.4](release-notes-0.18.4.md)
- [Garlicoin Core 0.18.3](release-notes-0.18.3.md)
- [Garlicoin Core 0.18.2](release-notes-0.18.2.md)

Older inherited Bitcoin and Litecoin release-note archives are intentionally not kept in this directory. They remain available through Git history and the respective upstream projects.

Project information
-------------------

The [root README](../README.md) describes the maintained fork, current release, supported artifacts, and CI status.

- [Contributing](../CONTRIBUTING.md)
- [Asset attribution](assets-attribution.md)
- [MIT license](../COPYING)
