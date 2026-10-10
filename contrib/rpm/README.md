Legacy RPM packaging
====================

The RPM spec and SELinux policy files previously stored here were inherited from old Bitcoin packaging, including a Bitcoin 0.12 LibreSSL patch. They are not used by the maintained Garlicoin Core 0.18.x GitHub Actions release path and no longer describe the current Qt/OpenSSL dependency baseline.

They have been removed rather than kept as apparently supported packaging instructions.

This directory remains only because `Makefile.am` still includes `contrib/rpm` in source-distribution metadata. A future build-system cleanup may remove that reference entirely.
