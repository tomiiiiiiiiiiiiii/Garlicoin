Shared libraries
================

bitcoinconsensus
----------------

Garlicoin Core retains the historical `bitcoinconsensus` library and symbol names inherited from upstream. The names are kept for source/API compatibility; in this repository the library verifies scripts using the Garlicoin Core consensus implementation.

The public C interface is defined in:

`src/script/bitcoinconsensus.h`

API version
-----------

`bitcoinconsensus_version()` returns the API version. The current header defines `BITCOINCONSENSUS_API_VER` as `1`.

Script verification
-------------------

The library exposes two verification entry points:

- `bitcoinconsensus_verify_script(...)`
- `bitcoinconsensus_verify_script_with_amount(...)`

They return `1` when the selected transaction input correctly spends the supplied `scriptPubKey` under the requested verification flags, and `0` otherwise.

The amount-aware function is required when witness verification needs the previous output amount.

Verification flags
------------------

The current public header defines flags for:

- P2SH;
- strict DER signatures;
- NULLDUMMY;
- CHECKLOCKTIMEVERIFY;
- CHECKSEQUENCEVERIFY;
- witness validation;
- the combined `VERIFY_ALL` set.

Errors
------

The error enum distinguishes successful parameter handling from transaction-index, transaction-size, deserialization, missing-amount, and invalid-flag errors.

Source of truth
---------------

Do not rely on a copied list of numeric flag values or API details from older Bitcoin/Litecoin documentation. For integrations, use the current `src/script/bitcoinconsensus.h` in the exact Garlicoin Core release being linked.
