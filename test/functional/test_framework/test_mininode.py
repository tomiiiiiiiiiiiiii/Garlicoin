#!/usr/bin/env python3
# Copyright (c) 2026 The Garlicoin Core developers
# Distributed under the MIT software license, see COPYING.
"""Transport regression tests; run with PYTHONPATH=test/functional python3 -m
unittest test_framework.test_mininode. No daemon or chain changes required.
"""
import socket
import struct
import unittest

from test_framework.messages import msg_ping, sha256
from test_framework.mininode import (MAGIC_BYTES, P2PInterface,
    network_thread_start, network_thread_join)


def frame(command, payload):
    return (MAGIC_BYTES['regtest'] + command.ljust(12, b'\0') +
            struct.pack('<I', len(payload)) + sha256(sha256(payload))[:4] + payload)


class TransportTest(unittest.TestCase):
    def test_fragmented_messages_and_restart(self):
        # Exercise pre-start buffering, fragmented/coalesced reads, automatic
        # pong, cross-thread writes, remote EOF, and repeated loop lifetimes.
        for _ in range(2):
            listener = socket.socket()
            listener.bind(('127.0.0.1', 0))
            listener.listen()
            listener.settimeout(5)
            peer = P2PInterface()
            peer.peer_connect('127.0.0.1', listener.getsockname()[1], send_version=False)
            peer.send_message(msg_ping(123), pushbuf=True)
            network_thread_start()
            try:
                conn, _ = listener.accept()
                conn.settimeout(5)
                with conn:
                    received = b''
                    expected = frame(b'ping', struct.pack('<Q', 123))
                    while len(received) < len(expected):
                        received += conn.recv(4096)
                    self.assertEqual(received, expected)
                    raw = frame(b'ping', struct.pack('<Q', 456))
                    conn.sendall(raw[:7])
                    conn.sendall(raw[7:] + raw)
                    expected = frame(b'pong', struct.pack('<Q', 456)) * 2
                    received = b''
                    while len(received) < len(expected):
                        received += conn.recv(4096)
                    self.assertEqual(received, expected)
                    peer.send_message(msg_ping(789))
                    expected = frame(b'ping', struct.pack('<Q', 789))
                    received = b''
                    while len(received) < len(expected):
                        received += conn.recv(4096)
                    self.assertEqual(received, expected)
            finally:
                listener.close()
                peer.peer_disconnect()
                network_thread_join()
            self.assertEqual(peer.state, 'closed')

    def test_refused_connection_and_empty_loop(self):
        with socket.socket() as reserved:
            reserved.bind(('127.0.0.1', 0))
            peer = P2PInterface()
            peer.peer_connect('127.0.0.1', reserved.getsockname()[1], send_version=False)
            network_thread_start()
            network_thread_join()
            self.assertEqual(peer.state, 'closed')
        network_thread_start()
        network_thread_join()

    def test_disconnect_before_start(self):
        with socket.socket() as listener:
            listener.bind(('127.0.0.1', 0))
            listener.listen()
            listener.settimeout(5)
            peer = P2PInterface()
            peer.peer_connect('127.0.0.1', listener.getsockname()[1], send_version=False)
            peer.peer_disconnect()
            network_thread_start()
            with listener.accept()[0] as conn:
                conn.settimeout(5)
                self.assertEqual(conn.recv(1), b'')
            network_thread_join()
            self.assertEqual(peer.state, 'closed')


if __name__ == '__main__':
    unittest.main()
