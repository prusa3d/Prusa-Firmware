#!/usr/bin/env python3

"""
Portable script to update po files on most platforms
"""

import argparse
from sys import stderr, exit
import shutil
from pathlib import Path
import polib
from polib import POFile

BASE_DIR: Path = Path.absolute(Path(__file__).parent)
PO_DIR: Path = BASE_DIR / "po"
PO_FILE_LIST: list[Path] = []
POT_REFERENCE: POFile = polib.pofile(PO_DIR/'Firmware.pot')


def main():
    global PO_FILE_LIST
    ap = argparse.ArgumentParser()
    group = ap.add_mutually_exclusive_group(required=True)
    group.add_argument('-f', '--file', help='File path for a single PO file to update Example: ./po/Firmware_cs.po')
    group.add_argument('--all', action='store_true', help='Update all PO files at once')
    args = ap.parse_args()

    if args.all:
        PO_FILE_LIST = sorted(PO_DIR.glob('**/*.po'))
    elif args.file:
        if Path(args.file).is_file():
            PO_FILE_LIST.append(Path(args.file))
        else:
            print("{}: file does not exist or is not a regular file".format(args.file), file=stderr)
            return 1

    for po_file in PO_FILE_LIST:
        # Start by creating a back-up of the .po file
        po_file_bak = po_file.with_suffix(".bak")
        shutil.copy(PO_DIR / po_file.name, PO_DIR / po_file_bak.name)
        po = polib.pofile(po_file)
        po.merge(POT_REFERENCE)
        po.save(po_file)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        exit(-1)
