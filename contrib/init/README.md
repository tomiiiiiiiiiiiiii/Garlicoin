Legacy service templates
========================

The historical `bitcoind` systemd, OpenRC, Upstart, SysV and launchd templates that previously lived here were inherited from upstream and did not match the maintained Garlicoin executable names, paths or release process.

They have been removed rather than shipped as misleading deployment examples.

This directory remains because `Makefile.am` still includes `contrib/init` in source-distribution metadata. A future build-system cleanup may remove the directory reference entirely.

For a current deployment, create service configuration explicitly for `garlicoind` and verify the data directory, configuration path, user permissions and RPC exposure for the target system.
