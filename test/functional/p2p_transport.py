#!/usr/bin/env python3
# Copyright (c) 2026 The Garlicoin Core developers
# Distributed under the MIT software license, see COPYING.
"""Validate the P2P transport lifecycle without mining or cached chain fixtures."""
from test_framework.mininode import P2PInterface, network_thread_start, network_thread_join
from test_framework.test_framework import BitcoinTestFramework


class TransportSmokeTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 1
        self.setup_clean_chain = True

    def run_test(self):
        for _ in range(2):
            peer = self.nodes[0].add_p2p_connection(P2PInterface())
            network_thread_start()
            try:
                peer.wait_for_verack()
                peer.sync_with_ping()
                assert self.nodes[0].getblockcount() == 0
            finally:
                self.nodes[0].disconnect_p2ps()
                network_thread_join()
            assert peer.state == 'closed'


if __name__ == '__main__':
    TransportSmokeTest().main()
