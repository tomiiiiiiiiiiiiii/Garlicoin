#!/usr/bin/env python3
# Copyright (c) 2012-2018 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Backward-compatible entry point for Garlicoin key/address test-vector generation."""

import os
import runpy
import sys

script_dir = os.path.dirname(os.path.abspath(__file__))
framework_dir = os.path.normpath(os.path.join(script_dir, "../../test/functional/test_framework"))
if framework_dir not in sys.path:
    sys.path.insert(0, framework_dir)

runpy.run_path(os.path.join(script_dir, "gen_key_io_test_vectors.py"), run_name="__main__")
