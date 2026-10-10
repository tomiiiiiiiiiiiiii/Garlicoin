Tor support
===========

Garlicoin Core can use a Tor SOCKS proxy for outbound network connections.

Proxying outbound connections
-----------------------------

Run a Tor SOCKS proxy locally and start Garlicoin Core with `-proxy`. A common Tor daemon configuration listens on port 9050:

```sh
garlicoind -proxy=127.0.0.1:9050
```

The actual SOCKS port depends on the Tor installation, so use the port configured by your local Tor client.

When `-proxy` is used, normal peer connections can be routed through Tor. Options such as `-connect`, `-addnode`, and `-seednode` continue to control peer selection.

P2P ports
---------

The current default Garlicoin P2P ports are:

- mainnet: `42069`;
- testnet: `42075`;
- regtest: `19444`.

These are P2P ports, not RPC ports.

Important legacy onion-service limitation
-----------------------------------------

The Garlicoin Core 0.18.x networking code inherited the older Bitcoin/Litecoin Tor implementation. Its native `.onion` address representation supports the historical v2 onion format, and its automatic `ADD_ONION` controller requests an `RSA1024` service key.

Modern Tor no longer supports v2 onion services. As a result, the old instructions for automatic hidden-service creation and direct v2 `.onion` peer addresses should not be treated as supported with current Tor releases.

Until Tor v3 onion-address support is deliberately ported and tested in Garlicoin Core, use Tor primarily as a SOCKS proxy for ordinary outbound peer connections. Do not rely on the legacy automatic onion-service feature for a modern deployment.

Relevant options
----------------

The source tree still contains the inherited Tor options, including:

- `-proxy` for general SOCKS proxying;
- `-onion` for the dedicated onion proxy path;
- `-listenonion` for automatic onion-service behavior;
- `-torcontrol` and `-torpassword` for the Tor control connection.

The presence of these options does not remove the v2/v3 compatibility limitation described above.

Source of truth
---------------

Tor address handling is implemented in `src/netaddress.cpp`, and Tor control/automatic service creation is implemented in `src/torcontrol.cpp`. Network P2P ports are defined in `src/chainparams.cpp`.
