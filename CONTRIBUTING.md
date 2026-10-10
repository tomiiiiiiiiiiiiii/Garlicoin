# Contributing to Garlicoin Core

Garlicoin Core is maintained as a compatibility-focused 0.18.x codebase. Contributions are welcome, but changes should be small, reviewable, and conservative.

## Project priorities

The maintenance line must preserve compatibility with the existing Garlicoin network and user data. Changes must not silently alter:

- consensus rules or network parameters;
- existing blockchain or datadir compatibility;
- wallet compatibility, including Berkeley DB 4.8 wallets;
- existing private keys, addresses, or transaction semantics.

Large forward-ports from newer Bitcoin Core or Litecoin Core versions are intentionally avoided. When upstream code is useful, prefer a small, clearly justified backport.

## Contributor workflow

1. Fork the repository.
2. Create a focused topic branch.
3. Make one logical change at a time.
4. Add or update tests where appropriate.
5. Run the relevant local checks.
6. Open a pull request against `master`.

Keep formatting-only changes separate from behavioral changes. Dependency updates, build-system changes, GUI changes, consensus-sensitive changes, and release tooling changes should normally be isolated into separate pull requests.

## Pull requests

A pull request should explain:

- what problem it solves;
- why the change is needed;
- what compatibility assumptions it preserves;
- what tests were run;
- any known limitations or platform-specific behavior.

Prefer small pull requests that are easy to review and revert. Avoid unrelated cleanup in functional changes.

Useful title prefixes include:

- `build:` build system or dependency changes
- `ci:` GitHub Actions and validation
- `doc:` documentation
- `net:` networking and P2P
- `qt:` GUI changes
- `rpc:` RPC, REST, or ZMQ
- `test:` tests and framework changes
- `wallet:` wallet code

## Testing and CI

Do not merge changes that knowingly break the maintained CI baseline unless the failure is documented and unrelated to the change.

Use the existing GitHub Actions workflows for platform validation. For release-sensitive changes, validate Linux, Windows, and macOS where applicable.

For developer details, see [`doc/developer-notes.md`](doc/developer-notes.md). Build instructions are available under [`doc/`](doc/).

## Consensus and compatibility changes

Changes that affect consensus, serialization, network behavior, address handling, wallet format, or chain parameters require substantially more review than ordinary maintenance work. Do not combine them with refactoring or dependency cleanup.

When in doubt, preserve existing behavior and discuss the change before expanding its scope.

## Licensing

By contributing to this repository, you agree that your contribution is provided under the MIT license unless a file explicitly states otherwise. Preserve existing copyright and attribution notices when modifying inherited code.
