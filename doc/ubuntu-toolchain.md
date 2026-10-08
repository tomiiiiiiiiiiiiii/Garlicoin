# Ubuntu toolchain validation

Ubuntu 22.04 remains the baseline. Ubuntu 24.04 is an additional native x86_64 GCC target; this is not a Core-version port or a new minimum supported OS.

## Reproduced failure and minimal fix

The initial [Ubuntu toolchain run](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/runs/37744707881) built pinned dependencies and configured successfully on both distributions. The 22.04 wallet build and four smoke tests passed. On 24.04, GCC 13 failed in `support/lockedpool.cpp`: `std::runtime_error` was undeclared, and the compiler identified the missing `<stdexcept>` header. Adding that direct include resolves the error without changing allocator logic. A standalone GCC 13/C++11 syntax check also fails before the include and passes after it.

Primary reference: Litecoin Core v0.18.1. Both its `src/support/lockedpool.cpp` and the later v0.21.2 translation unit were checked; neither directly includes `<stdexcept>`. This is a local header-compatibility fix, not a copy of a newer allocator. Qt 5.9.7 and its Litecoin-derived patches are inherited unchanged from PR #32.

The first GUI matrix run also reproduced `ModuleNotFoundError: No module named imp` while staging xcb-proto 1.10 on Python 3.12. Refresh only its bundled `py-compile` helper from the installed Automake (1.16.5 on the supported Ubuntu hosts). This uses the Automake upstream helper with its Python 3 importlib path, without regenerating unrelated configure logic or changing the pinned xcb-proto source. The old helper fails and the 1.16.5 helper successfully byte-compiles the same module under Python 3.12. Litecoin v0.18.1/v0.21.2 do not contain this refresh; it is a local recipe adjustment using an upstream build tool. Both normal and optimized byte-compilation retain failure propagation; the existing postprocess still removes bytecode.

## CI coverage

| Workflow | Ubuntu 22.04 | Ubuntu 24.04 |
| --- | --- | --- |
| Linux CI | Existing no-wallet CLI build, focused tests, wallet build and functional smoke | Not part of this workflow |
| Ubuntu toolchain CI | Wallet depends, configure, full build, ten focused tests, four smoke tests | Same coverage with native GCC and system Python |
| Qt GUI CI | Pinned Qt 5.9.7/BDB 4.8 depends, explicit GUI configure and complete GUI link | Same coverage; GUI is not silently disabled |

The focused test selection matches Linux CI. Functional tests are `wallet_encryption.py`, `rpc_uptime.py`, `rpc_net.py`, and `interface_bitcoin_cli.py`. Failures remain fatal. The existing manual-only full-unit diagnostic is outside the required PR checks.

Toolchain logs record GCC/G++, glibc, Autoconf, Automake, Libtool, pkg-config, Python, Perl, GNU Make, assembler/linker, and canonical triplets. Separate distribution cache keys prevent mixing the two Ubuntu stacks; depends additionally keys packages by toolchain identity. Complete dependency/build logs are retained as artifacts.

## Audit and limits

- `configure.ac`, `build-aux/`, depends recipes and native build documentation were inspected. Keep their existing warning/hardening checks; do not add permissive flags to bypass compiler errors.
- The bundled 2017 `config.guess` / `config.sub` recognize x86_64 Linux, and the initial 24.04 depends/configure steps pass. No age-only refresh is needed for this target.
- Autotools regeneration and dependency recipes exercise Perl and GNU Make. Native GCC/binutils/glibc build and final link are tested, not cross-distribution binary portability.
- Clang is not selected by these native Linux jobs; Darwin's separate pinned Clang/cctools path is outside this batch. No Clang support claim is made.
- System Python runs the selected RPC/wallet smoke tests. The broader legacy P2P harness imports `asyncore`, removed in Python 3.12; porting that harness is a separate follow-up, not covered by these four tests. No previously selected tests are removed.
- `contrib/install_db4.sh` and release/cross-platform helper scripts are not substituted for the validated depends path. Windows/macOS release builds remain separate work.
- Building pinned depends is not evidence of bit-for-bit reproducibility across distributions; that requires a separate reproducibility comparison.

Berkeley DB remains 4.8.30, OpenSSL 1.0.1k, and Qt 5.9.7. No consensus, Allium, DarkGravityWave, chainparams, genesis, address prefixes, emission, serialization, network protocol, wallet format, RPC semantics or live-network behavior changes. Existing wallet/datadir compatibility is preserved by the change scope; these CI tests do not claim a production-datadir migration test.

Check the current PR/commit Actions results before treating a new revision as validated.
