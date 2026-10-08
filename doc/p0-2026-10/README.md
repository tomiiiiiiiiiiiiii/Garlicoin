# P0 maintenance audit, 2026-10-08

## Scope and evidence

Base: `a5dcc512ae77f4f051527c9a6ac5e02935c7a028` (`master`). Primary upstream:
Litecoin Core **v0.18.1**, peeled commit `81c4f2d80fbd33d127ff9b31bf588e4925599d79`.
This is a selective source/exposure audit and a **partial executable baseline**, not
a clean bill of health or a claim that the full regression suite passes.

Production source, dependency versions, chain parameters, wallet database and wire
serialization are unchanged. See [dependencies](dependencies.md),
[upstream comparison](upstream.md), and [baseline](baseline.md).

Runtime evidence uses `release-linux-x86_64`, artifact **11555792587**, from
[Release Validation CI 37789446323](https://github.com/tomiiiiiiiiiiiiii/Garlicoin/actions/runs/37789446323),
head `8eb854990ab8dc88b200162d77e43d7b5c7eb753`. GitHub's comparison of that head to
the audit base contains only README changes. The executable reports
`v0.18.1.0-68cbe77` (PR merge build identity); do not mislabel it a locally built
binary at the audit commit. Tested using Python 3.12.14 on the provided Linux
execution environment. Compilers alone were present; autoconf, pkg-config, Boost
and Berkeley DB development headers were absent. `autogen.sh` and `make check`
were attempted and failed before compilation. Package installation failed on
user/group privilege operations. No full-unit result is inferred from smoke tests.

## P0.2 change and root cause

Before: importing `test_framework.mininode` fails at `import asyncore` on Python
3.12.14. Thus every direct/indirect consumer (P2P tests, comptool, several block
and validation tests) fails before reaching the daemon. RPC/wallet smoke tests
which do not import mininode are unaffected by this particular failure.

After: `asyncio.Protocol` and event-loop-owned transports, following the design in
Litecoin v0.18.1 `test/functional/test_framework/mininode.py`. This is deliberately
an adapter, not a wholesale upstream-file replacement: Garlicoin's existing tests
create all connections before starting `NetworkThread`, expect automatic thread
exit on the last disconnect, and sometimes join then restart it. Keep that API,
`state` and the legacy `connected` property, initial version buffering, callbacks, message map and lock. All transport
writes/aborts are scheduled on the loop thread. There is no third-party asyncore
replacement in the shipped framework.

Unchanged: message framing/checksum/parser logic (exception logging format corrected), `messages.py`, network magic,
protocol version, services, version/verack negotiation, production networking.
The existing `litecoin-scrypt==1.0.2` import remains necessary; it was separately
installed (with GCC) to run the tests. It is not a new runtime dependency.

Validation adds four transport unit tests (fragmentation/coalescing, buffering,
cross-thread sends, EOF, restart, malformed-frame disconnect, refused connection and cancellation before
start), plus `p2p_transport.py`: real daemon handshake/ping and reconnect at height
zero. Existing tests are retained. CI covers transport self-tests on Python 3.12
and 3.13; 3.13 passed in GitHub CI; it was not locally tested.

## Findings and next separate PRs

| Proposed work | Change risk | Priority / release gate |
|---|---|---|
| Repair legacy functional fixtures, especially Allium versus scrypt and cached-chain assumptions | MEDIUM | P0: prerequisite to trusting block/P2P validation coverage |
| Complete full unit diagnostic on a provisioned builder; retain every failure | LOW | P0: unresolved gate |
| GUI payment-protocol certificate/TLS mitigation, with hostile certificate tests | MEDIUM | P0: exposed legacy OpenSSL paths |
| Narrow OpenSSL link declarations and emit per-binary link maps | LOW | P1: removal of `-lssl` alone is not a security fix |
| OpenSSL 1.0.1-compatible security patch set or explicitly evaluated 1.0.1u experiment | MEDIUM | P0 candidate; 1.0.1u is not a supported modern endpoint |
| Move to a maintained OpenSSL major line | HIGH | Separate compatibility project, not an automatic bump |
| RPC running flag atomicity | LOW | Small upstream candidate; run shutdown/thread tests first |
| P2P oversized locator policy | MEDIUM | Needs explicit peer-behaviour review and negative P2P tests |
| Address-manager test-before-evict | HIGH | Separate anti-eclipse review, persistence and peer-selection tests |
| GUI parser stack (Qt, zlib, FreeType, Expat) hardening | MEDIUM | Confirm exact static/system library paths and inputs first |

Do not alter consensus, Allium, DGW, genesis, chainparams, subsidy/halving,
activation heights, network identity, prefixes, transaction/block serialization,
wallet.dat/BDB format or datadir layout to make inherited tests pass. In particular,
`fPowNoRetargeting` being true does not mean the active DGW path uses that flag;
changing it is outside this batch. No production backport was made without an
available build/test gate. No dependency bump is justified solely by age.

## Completion limits

P0.1 includes all depends recipes and source exposure mapping, but not a complete
2026 CVE/SBOM certification, exploit validation, or Windows/macOS runtime-link audit.
P0.2 resolves the Python import/transport blocker; it does not repair all inherited
functional tests. P0.3 remains incomplete wherever the baseline says environment
issue, historically broken or legacy test bug. P0.4 is the named, source-anchored
selection in the comparison table, not every historical commit or a forward-port.
