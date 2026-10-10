Unauthenticated REST interface
==============================

Garlicoin Core can expose a read-only REST interface when started with `-rest`.

The REST interface shares the HTTP/RPC server port for the selected network. The current defaults are:

- mainnet: `42068`;
- testnet: `42070`;
- regtest: `42070`.

The configured RPC port can override these defaults.

Supported endpoints
-------------------

The current implementation registers these REST prefixes:

- `/rest/tx/`
- `/rest/block/`
- `/rest/block/notxdetails/`
- `/rest/headers/`
- `/rest/chaininfo`
- `/rest/mempool/info`
- `/rest/mempool/contents`
- `/rest/getutxos`

Transactions
------------

```text
GET /rest/tx/<txid>.<bin|hex|json>
```

Returns a transaction in binary, hexadecimal, or JSON form. Historical transactions that are not otherwise available may require `-txindex=1`.

Blocks
------

```text
GET /rest/block/<block-hash>.<bin|hex|json>
GET /rest/block/notxdetails/<block-hash>.<bin|hex|json>
```

The `notxdetails` form omits full transaction details from JSON output.

Headers
-------

```text
GET /rest/headers/<count>/<block-hash>.<bin|hex|json>
```

`count` must be between 1 and 2000. Headers are returned forward from the requested block while it remains on the active chain.

Chain and mempool information
-----------------------------

```text
GET /rest/chaininfo.json
GET /rest/mempool/info.json
GET /rest/mempool/contents.json
```

These endpoints expose the corresponding current node state as JSON.

UTXO queries
------------

```text
GET /rest/getutxos[/checkmempool]/<txid>-<n>/... .<bin|hex|json>
```

The implementation accepts at most 15 outpoints per request. `checkmempool` includes the mempool view when evaluating the requested outpoints.

Security
--------

The REST interface is unauthenticated. Do not expose it to untrusted networks unless that exposure is intentional and protected externally. A local web browser or other untrusted local process may also be able to query an interface bound to localhost.

Source of truth
---------------

Endpoint registration and serialization behavior are implemented in `src/rest.cpp`. Network RPC-port defaults are defined in `src/chainparamsbase.cpp`. Those source files take precedence over this document if they change.
