# Regression baseline, 2026-10-08

**Incomplete diagnostic baseline.** Every runner entry is listed, including entries
that could not be executed. ENVIRONMENT ISSUE is not PASS and must not be used to
waive an observed assertion failure. A complete baseline still requires the full
unit suite and all blocked/inconclusive functional tests on provisioned builders.

Source and binary provenance are in README.md. Original /tmp diagnostic logs were
lost during the interruption; only results actually visible before interruption
are carried forward. Fresh smoke outputs are preserved in retained-smoke-logs.json.
No claim is made that the interrupted full run completed. Machine-readable entries:
functional-baseline.json.

## Verified CI and platform scope

| Test/build | Status | Evidence and limit |
|---|---|---|
| make check / complete unit suite locally | ENVIRONMENT ISSUE | autogen.sh: install autoconf first; make: no check target. No unit binary in release artifact. |
| Full unit CI diagnostic | SKIP — intentional | Linux CI makes this workflow_dispatch-only; not run on PR #38. Still an open P0 gate. |
| Ten focused unit selections | PASS | Linux CI 37821494717 and Ubuntu CI 37821494716 at ad46019; production sources unchanged by later transport correction. |
| Python transport tests 3.12 / 3.13 | PASS | P2P CI 37829348861 on corrected transport 5fcafd7. |
| Ubuntu 22.04 wallet/build/smoke | PASS | CI 37821494716; final correction CI is recorded separately in PR checks. |
| Ubuntu 24.04 wallet/build/smoke | PASS | Same CI, separate matrix job. |
| Windows MinGW x86_64 build | PASS | Existing production baseline CI 37796279788; PE64 verification, not Windows runtime tests. |
| macOS x86_64 cross-build | PASS | Existing baseline CI 37796279729; Mach-O verification, not native execution or proof of two-build reproducibility. |
| Qt GUI on Ubuntu 22.04/24.04 | PASS | Existing baseline CI 37796279601; compile/link verification, not interactive GUI tests. |
| Linux/Windows/macOS release packaging | PASS | Release Validation CI 37789446323: ELF/source tarball, PE64/installer, Mach-O/DMG. |
| Wallet format migration / old real datadir | ENVIRONMENT ISSUE | No user-owned historical fixture provided or exercised; format and production code unchanged. |

Focused selections: compress_tests, main_tests, base58_tests, key_tests/key_test1,
DoS_tests/stale_tip_peer_management, sighash_tests/sighash_from_data, pow_tests,
transaction_tests/tx_valid, tx_validationcache_tests/tx_mempool_block_doublespend,
miner_tests/CreateNewBlock_validity. These passes do not imply all other unit cases
pass. Baseline platform source is unchanged production code; no new platform build
was required solely for Python/report changes.

## Important triage

* The first asyncio implementation removed asyncore's `connected` property.
  p2p_timeouts initially failed with AttributeError: **FAIL — real regression**.
  Commit 5fcafd7 restores the property; the unchanged 62-second test now PASS.
* wallet_encryption has a timing-sensitive check: unlock for two seconds, sleep
  exactly two seconds, immediately require lock. Parallel rerun failed with
  `No exception raised`; isolated rerun passed. CI also passed. Its table entry
  records the isolated PASS; the parallel failure remains in retained logs and
  must be stabilized separately, not silently discarded.
* The active GetNextWorkRequired path invokes DGW directly; the BTC-style
  fPowNoRetargeting branch is not the active entry point. Existing cache/large
  mining tests must not be rescued by modifying consensus or Allium.
* Block fixtures still calculate litecoin_scrypt.getPoWHash. That cannot certify
  Allium block validation. Repair fixtures in a separate tested batch.

## Functional inventory

Counts below include unexecuted entries and are not an executed-suite success rate.

- FAIL — legacy test bug: 11
- ENVIRONMENT ISSUE: 68
- HISTORICALLY BROKEN: 2
- PASS: 13
- SKIP — intentional: 1

| Test | Status | Evidence / unresolved gate |
|---|---|---|
| `wallet_hd.py` | FAIL — legacy test bug | Unchanged helper asserts litecoind exited against garlicoind initialization error; wallet_hd reverified with retained log. |
| `wallet_backup.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `feature_block.py` | HISTORICALLY BROKEN | Failure observed on both old transport (CPython asyncore supplied for comparison only) and new transport. Original full logs lost during interruption; prior observed result retained, not a new PASS. |
| `rpc_fundrawtransaction.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `p2p_compactblocks.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `feature_segwit.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `wallet_basic.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `wallet_accounts.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `p2p_segwit.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `wallet_dump.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `rpc_listtransactions.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `p2p_sendheaders.py` | FAIL — legacy test bug | Existing block fixtures use scrypt; high-hash observed. Full causal/exploit validation still incomplete; no consensus change. |
| `wallet_zapwallettxes.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `wallet_importmulti.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `mempool_limit.py` | FAIL — legacy test bug | Inherited expected fee 0.00001000 versus Garlicoin 0.00100000. |
| `rpc_txoutproof.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `wallet_listreceivedby.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `wallet_abandonconflict.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `feature_csv_activation.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `rpc_rawtransaction.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `wallet_address_types.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `feature_reindex.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `wallet_keypool_topup.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `interface_zmq.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `interface_bitcoin_cli.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `mempool_resurrect.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `wallet_txn_doublespend.py --mineblock` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `wallet_txn_clone.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `wallet_txn_clone.py --segwit` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `rpc_getchaintips.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `interface_rest.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `mempool_spend_coinbase.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `mempool_reorg.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `mempool_persist.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `wallet_multiwallet.py` | FAIL — legacy test bug | Unchanged helper asserts litecoind exited against garlicoind initialization error; wallet_hd reverified with retained log. |
| `wallet_multiwallet.py --usecli` | FAIL — legacy test bug | Unchanged helper asserts litecoind exited against garlicoind initialization error; wallet_hd reverified with retained log. |
| `interface_http.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `rpc_users.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `feature_proxy.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `rpc_signrawtransaction.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `p2p_disconnect_ban.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `rpc_decodescript.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `rpc_blockchain.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `rpc_deprecated.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `wallet_disable.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `rpc_net.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `wallet_keypool.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `p2p_mempool.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `p2p_transport.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `mining_prioritisetransaction.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `p2p_invalid_block.py` | FAIL — legacy test bug | Existing block fixtures use scrypt; high-hash observed. Full causal/exploit validation still incomplete; no consensus change. |
| `p2p_invalid_tx.py` | FAIL — legacy test bug | Existing block fixtures use scrypt; high-hash observed. Full causal/exploit validation still incomplete; no consensus change. |
| `feature_versionbits_warning.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `rpc_preciousblock.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `wallet_importprunedfunds.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `rpc_signmessage.py` | FAIL — legacy test bug | Inherited signed-message fixture differs from Garlicoin signature; production sign-message code unchanged. |
| `feature_nulldummy.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `wallet_import_rescan.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `mining_basic.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `wallet_bumpfee.py` | SKIP — intentional | Exit 77 observed before interruption (feature/configuration skip). |
| `rpc_named_arguments.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `wallet_listsinceblock.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `p2p_leak.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `wallet_encryption.py` | PASS | Fresh serial/isolated rerun after connected API correction. |
| `wallet_scriptaddress2.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `feature_dersig.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `feature_cltv.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `rpc_uptime.py` | PASS | Fresh Python 3.12.14 run, release artifact 11555792587. |
| `wallet_resendwallettransactions.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `feature_minchainwork.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `p2p_fingerprint.py` | HISTORICALLY BROKEN | Failure observed on both old transport (CPython asyncore supplied for comparison only) and new transport. Original full logs lost during interruption; prior observed result retained, not a new PASS. |
| `feature_uacomment.py` | FAIL — legacy test bug | Unchanged helper asserts litecoind exited against garlicoind initialization error; wallet_hd reverified with retained log. |
| `p2p_unrequested_blocks.py` | ENVIRONMENT ISSUE | Exit 1 observed before interruption; cause not conclusively classified. Potential real regression remains an open gate; this label does not waive the failure. |
| `feature_logging.py` | FAIL — legacy test bug | Unchanged helper asserts litecoind exited against garlicoind initialization error; wallet_hd reverified with retained log. |
| `p2p_node_network_limited.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `feature_config_args.py` | FAIL — legacy test bug | Unchanged helper asserts litecoind exited against garlicoind initialization error; wallet_hd reverified with retained log. |
| `feature_btg_finalize_block.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `feature_pruning.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `feature_fee_estimation.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `feature_maxuploadtarget.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `mempool_packages.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `feature_dbcrash.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `feature_bip68_sequence.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `mining_getblocktemplate_longpoll.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `p2p_timeouts.py` | PASS | Fresh serial/isolated rerun after connected API correction. |
| `feature_bip9_softforks.py` | ENVIRONMENT ISSUE | 90-second diagnostic limit reached before interruption; inconclusive. Not a skip or pass. |
| `p2p_feefilter.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `rpc_bind.py` | ENVIRONMENT ISSUE | Exit 1 observed before interruption; cause not conclusively classified. Potential real regression remains an open gate; this label does not waive the failure. |
| `feature_assumevalid.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `example_test.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `wallet_txn_doublespend.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `wallet_txn_clone.py --mineblock` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |
| `feature_notifications.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `rpc_invalidateblock.py` | ENVIRONMENT ISSUE | Not verified: full diagnostic run interrupted; final raw logs unavailable. No PASS inferred. |
| `feature_rbf.py` | ENVIRONMENT ISSUE | Not executed: unavailable shared 200-block cache. Cache construction observed stalled after height 45; no production PoW/difficulty change allowed. |

## Reproduction

On a fully configured wallet build with the existing Python PoW dependency:

```sh
make check VERBOSE=1
PYTHONPATH=test/functional python3 -m unittest test_framework.test_mininode -v
python3 test/functional/test_runner.py --extended --jobs=1 --combinedlogslen=100
```

The full runner requires a valid cached-chain fixture. Do not interpret an import
fix or a cache creation failure as execution of every P2P test. For isolated
transport validation without mining/cache:

```sh
python3 test/functional/p2p_transport.py --srcdir=/absolute/path/to/bin
python3 test/functional/p2p_mempool.py --srcdir=/absolute/path/to/bin
python3 test/functional/p2p_timeouts.py --srcdir=/absolute/path/to/bin
```
