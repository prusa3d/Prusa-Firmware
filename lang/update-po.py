#!/usr/bin/env python3

"""
Portable script to update po files on most platforms
Make sure the .pot file is up-to-date first by
by calling within the ./po folder:
python ../lang-extract.py --no-missing -s -o Firmware.pot ../../Firmware/[a-zA-Z]*.[ch]* ../../Firmware/mmu2/[a-zA-Z]*.[ch]*
"""

import sys
from pathlib import Path
import polib
from polib import POFile

BASE_DIR: Path = Path.cwd()
PO_DIR: Path = BASE_DIR / "po"
PO_FILE_LIST: list[Path] = []
POT_REFERENCE: POFile = polib.pofile(PO_DIR/'Firmware.pot')


def main():
    PO_FILE_LIST = sorted(PO_DIR.glob('**/*.po'))

    for po_file in PO_FILE_LIST:
        po = polib.pofile(po_file)
        po.merge(POT_REFERENCE)
        po.save(po_file)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(-1)
