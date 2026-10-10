Developer notes
===============

These notes describe the maintenance conventions for this Garlicoin Core repository. They intentionally focus on practices that are relevant to the maintained 0.18.x line rather than preserving the full historical Bitcoin/Litecoin developer manual.

Maintenance scope
-----------------

Routine maintenance should keep the existing Garlicoin network compatible. Do not mix ordinary build, dependency, GUI, documentation, or test work with changes to:

- consensus rules;
- Allium proof of work;
- chain parameters or network identity;
- address formats;
- transaction/block serialization;
- existing datadir compatibility;
- the Berkeley DB 4.8 wallet format.

Changes in those areas require separate, explicit review and testing.

Garlicoin is Litecoin-derived. Litecoin Core 0.18.1 is the primary upstream reference for this maintenance generation. Newer Bitcoin/Litecoin code can be consulted for individual fixes, but should not be forward-ported wholesale.

Patch discipline
----------------

- Keep changes narrowly scoped.
- Avoid unrelated formatting churn.
- Prefer the smallest fix that addresses the observed issue.
- Do not update an old dependency merely because it is old; establish the compatibility and security reason first.
- Add or update tests when behavior changes.
- Keep source-code behavior and documentation in the same change when practical.

Style
-----

Follow `src/.clang-format` and the style already established by the surrounding maintained code.

General rules:

- four-space indentation;
- prefer `nullptr` over `NULL` in new code;
- prefer RAII and scoped ownership;
- initialize fields explicitly;
- avoid side effects in assertions;
- include headers for symbols used directly instead of depending on accidental transitive includes;
- avoid `using namespace ...` in headers and global scope;
- use fixed-width integer types when representation matters.

Do not submit a patch solely to restyle unrelated existing code.

Build and dependency work
-------------------------

For CI/release-equivalent builds, prefer the pinned `depends` stack. The package recipes under `depends/packages/` are the source of truth for dependency versions and platform-specific build options.

The current maintenance baseline includes Qt 5.15.19, OpenSSL 3.5.9, and Berkeley DB 4.8.30 wallet compatibility.

Do not weaken hardening or compiler diagnostics simply to make an unsupported dependency/toolchain combination compile.

Testing
-------

At minimum, use the tests relevant to the changed area. Common entry points include:

```sh
make check
python3 test/functional/test_runner.py
```

For smaller changes, focused unit/functional tests are acceptable during development, but release work should use the repository's GitHub Actions validation.

The maintained workflows under `.github/workflows/` are authoritative for current CI coverage. Do not rely on historical Travis/Gitian instructions.

When debugging test failures, keep track of whether a failure is newly introduced, historically broken, skipped by design, or environment-specific.

Debugging
---------

Useful development options include:

```sh
./configure --enable-debug
```

`debug.log` is the primary runtime diagnostic log. Use the existing `-debug=<category>` options to narrow logging where possible.

For lock-order investigation, build with `DEBUG_LOCKORDER` support and treat new lock-order warnings as defects rather than suppressing them.

Wallet code
-----------

- Code must continue to build and run with wallet support disabled where that configuration is supported.
- Do not include Berkeley DB headers unconditionally in non-wallet code.
- Treat wallet database compatibility as a release constraint.
- Do not casually change serialization or database behavior in dependency/build patches.

Threads and locking
-------------------

The codebase is multi-threaded and uses `LOCK`/`TRY_LOCK`-style synchronization extensively.

- Keep lock lifetimes visually scoped.
- Avoid lock-order inversions.
- Do not assume read-only-looking container access is thread-safe if the API can insert or mutate state.
- Prefer existing synchronization conventions in the affected subsystem over introducing an unrelated concurrency abstraction in a small patch.

C++ data handling
-----------------

- Prefer `.find()` for map lookups when no insertion is intended.
- Avoid out-of-bounds vector access and pointer arithmetic on empty containers.
- Use the parsing helpers in `utilstrencodings.h` for numeric input where applicable.
- Use `AmountFromValue` and `ValueFromAmount` for monetary JSON/RPC conversion rather than floating-point shortcuts.
- Be careful to distinguish `LogPrint` category logging from `LogPrintf`.

RPC changes
-----------

Keep RPC interfaces consistent with the existing 0.18.x API:

- use established lowercase method naming;
- use `snake_case` argument names;
- use JSON parsing rather than hand-written string parsing where possible;
- treat missing and explicit `null` consistently when an argument is optional;
- update the RPC command/conversion tables when adding non-string arguments;
- avoid unnecessary type-overloaded behavior that is difficult to use from `garlicoin-cli`;
- prefer extensible JSON objects for new structured responses.

GUI changes
-----------

Keep presentation logic in Qt view/controller code and avoid pushing dialog/UI behavior into core model classes.

The current maintenance direction preserves the original wallet appearance while fixing concrete compatibility/usability issues such as High-DPI behavior. Avoid broad redesigns in unrelated maintenance patches.

Functional-test framework
-------------------------

The P2P functional-test transport now uses `asyncio` while retaining the existing test-facing interface. Do not reintroduce the removed Python `asyncore` dependency.

Source files may retain historical upstream names such as `test_bitcoin.cpp` or `bitcoinconsensus.h`. Renaming those identifiers is not by itself a maintenance goal; distinguish harmless inherited internal names from user-facing or misleading documentation.

Review and release
------------------

Before merging a maintenance change:

1. inspect the diff for unrelated changes;
2. run the relevant local/focused tests;
3. confirm the applicable GitHub Actions checks;
4. update durable documentation when the supported build/release behavior changed.

Before a release, follow [release-process.md](release-process.md) and validate the exact release revision across the maintained Linux, Windows, and macOS x86_64 build paths.
