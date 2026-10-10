Translation strings policy
==========================

This document describes the maintained translation conventions for Garlicoin Core.

Marking strings for translation
-------------------------------

- In GUI code under `src/qt`, use Qt translation facilities such as `tr("...")`.
- In non-GUI user-facing code under `src`, use `_("...")` where the surrounding code already uses the gettext-style translation wrapper.
- Developer-only diagnostics, internal logs, RPC field names, and protocol/internal error details generally should not be translated.

What should be translated
-------------------------

Translate text that is directly presented to users, including:

- window titles, labels, buttons, menus and tooltips;
- user-facing progress and message-box text;
- normal command-line option descriptions.

Avoid putting changing default values directly into translation strings. Format the value into the translated message instead so translators do not need to update a string solely because a default changed.

Writing translation-friendly strings
------------------------------------

- Prefer complete, self-contained sentences instead of fragments assembled at runtime.
- Avoid unnecessary near-duplicate strings.
- Avoid embedding presentation HTML when normal Qt formatting can be used instead.
- Keep detailed developer/debug information in logs and show users a concise translatable message when appropriate.

Plurals
-------

Use Qt's numerus support for GUI strings that depend on a count. For example:

```cpp
tr("%n active connection(s) to the Garlicoin network", "", count);
```

Qt translation files can then provide the language-specific plural forms.

Updating translations
---------------------

The source tree retains historical upstream filenames for some translation infrastructure. In particular, the English Qt catalog is currently named:

`src/qt/locale/bitcoin_en.ts`

The filename is an inherited implementation detail and does not mean the application is Bitcoin Core.

When changing translatable strings, use the translation/update targets supported by the current build tree and review the resulting `.ts` changes before committing them. Do not perform unrelated mass translation churn as part of a small maintenance patch.
