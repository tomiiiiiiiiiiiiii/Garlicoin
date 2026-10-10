Expectations for DNS seed operators
===================================

Garlicoin Core attempts to minimize trust in DNS seeds, but DNS seeds remain part of initial peer discovery and therefore carry some operational responsibility.

These expectations apply to operators of DNS seeds used by Garlicoin software.

1. Seed results should contain only reasonably selected, reachable Garlicoin nodes from the public network to the best of the operator's knowledge and capability.
2. Results may be randomized, but operators should not selectively return different peer populations except for a documented technical reason.
3. DNS responses should not use a TTL shorter than one minute without a specific operational reason.
4. Query logging should be limited to what is necessary for operation or urgent network-health investigation and should not be retained longer than necessary.
5. Data gathered by public-node crawling may be retained or published, provided the crawler does not deliberately bias connectivity to make that dataset artificially complete.
6. Operators are encouraged to document relevant operating practices.
7. A reachable contact method should be available for operational or security reports concerning the seed.
8. Control of a seed should not be silently sold or transferred to an unrelated operator. If responsibility changes, the Garlicoin maintainers should be informed so the seed can be reviewed.

If an operator can no longer meet these expectations, the seed should be withdrawn or the active Garlicoin maintainers should be contacted.

For issues concerning seeds used by this maintained source tree, open an issue in the maintained repository:

https://github.com/tomiiiiiiiiiiiiii/Garlicoin/issues

The actual seed list used by a release is defined in `src/chainparams.cpp`; that source file is authoritative.
