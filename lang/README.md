# Internationalization support

Multi-language support in the Prusa MK3 firmware is based on PO language files.

Firmware support is controlled by the ``LANG_MODE`` define in the configuration, which defaults to 1 (enabled). When ``LANG_MODE`` is set, the firmware *can* load additional languages, but these extra languages need to be "baked in" in the firmware image for flashing. This last step is performed using the tools in this directory.

## Quick reference

### Required tools

Python 3 is the main tool. To install the required packages run the following command in the `lang` folder:

    pip install -r requirements.txt

### Main summary

Language files:

* ``po/Firmware.pot``: Main list of strings to translate (do *not* change this file manually - it is automatically generated)
* ``po/Firmware_XY.po``: Translations for "XY", where XY is the ISO639-1 language code.

PO files are simple text files that can be edited with any text editor, but are best handled using dedicated tools such as POEdit, Lokalize or Linguist.

High-level tools:

* ``config.sh``: Language selection/configuration
* ``fw-build.sh``: Builds the final multi-language hex file into this directory
* ``fw-clean.sh``: Cleanup temporary files left by ``fw-build.sh``
* ``update-pot.py``: Extract internationalized strings from the sources and place them inside ``po/Firmware.pot``
* ``update-po.py``: Refresh po file/s with new translations from the main pot file.

Lower-level tools:

* ``lang-check.py``: Checks a single po file for screen formatting issues.
* ``lang-extract.py``: Extract internationalized strings from source files.
* ``lang-map.py``: Extract and patch the translation symbol map in the firmware.
* ``lang-build.py``: Build a binary language catalog for a single po file.
* ``lang-patchsec.py``: Embed a single secondary language catalog in the firmware.
* ``lib/charset.py``: Tool for converting unicode strings to the lcd font and back.
* ``lib/FontGen.py``: Tool for generating the font table used by the firmware.

### Building an internationalized firmware

This is accomplished by running ``fw-build.sh`` after building the firmware with ``LANG_MODE = 1``. Language selection is done by modifying ``config.sh``.

After running the script, the final ``Firmware-intl.hex`` will be generated in this directory.

This step is already performed for you when using ``build.sh`` or ``PF-build.sh``. You can however re-run ``fw-build.sh`` to update just the language catalogs inside the image.

### Updating an existing translation

#### How to update `.pot` file

Run 

     python update-pot.py

to regenerate ``po/Firmware.pot`` and verify that the annotation has been picked up correctly. You can stop here if you only care about the annotation.

#### How to update `.po` file

To update a single `.po` file:

    python update-po.py --file Firmware_XY.po

This will propagate the new strings to your language. This will merge the new strings, update references/annotations as well as marking unused strings as obsolete.

To update all .po files at once:

    python update-po.py --all



#### Typo or incorrect translation in existing text

If you see a typo or an incorrect translation, simply edit ``po/Firmware_XY.po`` and make a pull request with the changes.

You can use the following command:

    ./lang-check.py po/Firmware_XY.po

to check for screen formatting issues in isolation, or use the ``--information`` flag:

    ./lang-check.py --information po/Firmware_XY.po

to preview all translations as formatted on the screen.

#### Missing translation without entry in po file

If some text is missing, but there is no reference text in the po file, you need to refresh the translation file by picking up new strings and annotations from the template.

See section [how to update .po file](#how-to-update-.po-file) to update the translations, then proceed as for [typo or incorrect translation](#typo-or-incorrect-translation-in-existing-text).

### Fixing an incorrect screen annotation or english text

The screen annotations as well as the original english text is extracted from the firmware sources. **Do not change the main pot file**. The ``pot`` and ``po`` file contains the location of the annotation to help you fix the sources themselves.

* See section [how to update .pot file](#how-to-update-.pot-file) to update the reference file.

* To sync one language: See section [how to update .po file](#how-to-update-.po-file); to propagate the annotation from the `.pot` file to your language, then proceed as for [typo or incorrect translation](#typo-or-incorrect-translation-in-existing-text).

### Adding a new language

Each language is assigned a two-letter ISO639-1 language code.

The firmware needs to be aware of the language code. It's probably necessary to update the "Language codes" section in ``Firmware/language.h`` to add the new code as a ``LANG_CODE_XY`` define as well as add the proper language name in the function ``lang_get_name_by_code`` in ``Firmware/language.c``.

It is a good idea to ensure the translation template is up-to-date before starting to translate. See section [how to update .pot file](#how-to-update-.pot-file).

Copy ``po/Firmware.pot`` to ``po/Firmware_XY.po``. The *same* language code needs to be used for the "Language" entry in the metadata. Other entries can be customized freely.

The new language needs to be explicitly added to the list of bundled languages in ``config.sh``.

At this point the new language should be picked-up normally. See [how to build an internationalized firmware](#building-an-internationalized-firmware) and use ``lang-check.py`` for [previewing the translation](#typo-or-incorrect-translation-in-existing-text) without having to perform a complete rebuild.

### Diffing translations

po/pot files contain file+line annotations for each translation which are convenient while translating but result in unwieldy diffs.

To alleviate the problem when using git you can add the following snippet to your ``~/.config/git/config`` or ``~/.gitconfig`` file to automatically remove position changes using the ``msgcat`` utility:

```
[diff "po"]
        textconv = msgcat --no-location --sort-output
```

This requires ``gettext`` to be installed.

## Diacritics

The font used for all the diacritics is stored in `lib/Prusa.lcd`. This file was created using `GLCD font creator`. In order to edit an existing character or to create a new one, this tool must be used. Beware that the tool is old and is windows only. In order to be able to save the .lcd file, the program must be started with Windows XP SP3 compatibility mode.

The .lcd file contains all diacritics and custom characters, but not all are used in the firmware. The ones which are used are defined in `lib/FontGen.py`. This script is used for generating the font table used inside the firmware and it must be executed whenever the .lcd file is changed to regenerate the font data.

Since some of the diacritics are not going to be implemented at all, there is a preprocessing pass on the translated strings that replaces unimplemented characters with ones that are impelemented. This replacing is done in `lib/charset.py`.

In terms of limitations, there can only be at most 96 custom characters defined in the firmware (0x80-0xDF) and at any moment, only 8 distinct custom characters can be displayed on the screen. This is a limitation of the hardware, not of the software. Since some translations might use more than 8 diacritics on the screen, an "alternate" form of each diacritic is implemented. This alternate form is the closest representation from the LCD's hardcoded font.

## Internal details

TODO
