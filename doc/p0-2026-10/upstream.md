# Selective Litecoin Core 0.18.1 robustness comparison

Reference for every row: immutable tag commit
[`81c4f2d80fbd33d127ff9b31bf588e4925599d79`](https://github.com/litecoin-project/litecoin/tree/81c4f2d80fbd33d127ff9b31bf588e4925599d79).
Paths/functions below identify the reviewed upstream implementation, not invented
backport commit IDs. Comparison is against Garlicoin `a5dcc51`. No entire source
file was synchronized. Categories describe the specific mechanism, not overall
subsystem security. Risk rates the proposed further change, not severity.

| upstream fix | subsystem | present in Garlicoin? | applicable? | compatibility risk | recommendation |
|---|---|---|---|---|---|
| `mininode.py`: asyncio.Protocol transport | P2P tests | no at base | applicable | LOW, test-only | adapted in this batch; preserve legacy lifecycle and messages |
| `netbase.cpp`: IsSelectableSocket before select/connect | sockets | yes, InterruptibleRecv and CreateSocket paths | already present | LOW | retain; no cosmetic port |
| `net.cpp`: optional poll-based event handling | sockets | no, select loop retained | candidate for backport | MEDIUM | separate descriptor scalability/platform test project |
| `net_processing.cpp`: bounded inv/getdata lists and headers count | P2P resource limits | yes, MAX_INV_SZ/MAX_HEADERS_RESULTS checks | already present | LOW | retain; no wire format change |
| `net_processing.cpp`: MAX_LOCATOR_SZ on getblocks/getheaders | P2P resource limits | no corresponding checks found | candidate for backport | MEDIUM | negative test first; explicitly review stricter peer disconnect policy |
| `net_processing.cpp`: stale-tip outbound peer eviction/protection | anti-stall/DoS | yes, CheckForStaleTipAndEvictPeers and m_chain_sync protection | already present | LOW | retain focused stale_tip_peer_management test |
| `net_processing.cpp`: random orphan vector and indexed erase | resource/DoS | old map-based mechanism instead | too risky / low value | MEDIUM | benchmark/DoS evidence before coupled container refactor |
| `net_processing.cpp`: sanitized received/unknown-command log text | logging | SanitizeString on command paths present | already present | LOW | keep; this is not proof every logged field is safe |
| `addrman.h/.cpp`: tried collisions and test-before-evict | addrman/anti-eclipse | no m_tried_collisions/test-before-evict machinery | candidate for backport | HIGH | separate policy and persistence review; not a mechanical port |
| `addrman.h`: lock annotations on maps and helpers | threading analysis | sparse/absent vs upstream | candidate for backport | LOW | annotation-only batch with supported compiler checks |
| `torcontrol.cpp`: MAX_LINE_LENGTH and bounded binary-file reads | Tor/resource bounds | present | already present | LOW | retain current checks |
| `torcontrol.cpp/.h` + `test/torcontrol_tests.cpp`: expose parser helpers for testing | Tor parsing coverage | helpers static, no equivalent upstream test file | candidate for backport | LOW | isolate visibility/test changes; no Tor protocol rewrite |
| `torcontrol.cpp`: std::bind replacement of boost::bind | portability | Boost binding retained and modern placeholders used | too risky / low value | LOW | no security benefit established; keep validated code |
| `httpserver.cpp`: require rpcbind plus rpcallowip before non-loopback binds | HTTP RPC exposure | already implemented, with Garlicoin-specific wording | already present | LOW | retain; do not weaken default binding |
| `httpserver.cpp`: Connection: close on replies after ShutdownRequested | HTTP shutdown | absent | candidate for backport | MEDIUM | test keep-alive clients and concurrent shutdown first |
| `httpserver.cpp`: newer event-thread shutdown/drain design | threading/shutdown | older future/2-second loopbreak design | too risky / low value | MEDIUM | don't replace without reproduced hang and regression test |
| `rpc/server.cpp`: atomic RPC running flag | RPC concurrency | plain static bool fRPCRunning | candidate for backport | LOW | source demonstrates unsynchronized flag; separate atomic-only patch with shutdown tests |
| atomic shutdown request in init/shutdown implementation | shutdown | std::atomic<bool> fRequestShutdown already present | already present | LOW | no reason to duplicate newer shutdown file architecture |
| `wallet/db.cpp`: BerkeleyEnvironment/Database per-environment lifetime and unique file-id handling | wallet/resource ownership | older global CDBEnv/CDB wrappers | too risky / low value | HIGH | review individual alias/recovery bugs; do not transplant wallet storage architecture |
| `qt/paymentrequestplus.cpp`: RAII around all EVP/X509 temporary ownership | GUI/resource handling | not complete in either compared file | not applicable | MEDIUM | do not claim 0.18.1 already fixes this; independently review cleanup paths if proposing a fix |
| `serialize.h`: bounded CompactSize/container allocation | parsing/validation | existing bounds present | already present | HIGH if altered | keep wire serialization unchanged; additional fuzz tests may be separate |
| newer Litecoin script/consensus activation changes | consensus | intentionally fork-specific | not applicable | HIGH | never import as maintenance fixes |
| full newer wallet/RPC architecture and added methods | wallet/RPC | diverged | too risky / low value | HIGH | no broad API/format replacement |
| direct stdexcept include for modern compiler in lockedpool.cpp | portability | Garlicoin local maintenance fix present | already present | LOW | no upstream copy needed; v0.18.1 lacks this direct include |

No production candidate was implemented in this batch: the environment could not
compile unit binaries and some inherited functional fixtures are already broken.
A source-level improvement is not a substitute for the requested validation gate.
Existing wire behaviour and wallet/database compatibility take precedence.

Review coverage includes selected networking, socket, Tor, addrman, HTTP/RPC,
wallet database ownership, shutdown, logging, parsing and compiler paths. This
is not an exhaustive commit-by-commit security audit. In particular, absence of
a row does not establish equivalence or absence of a vulnerability.
