#!/usr/bin/env python3
import argparse
import elftools.elf.elffile

from lib.io import fatal


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('elf', help='Firmware ELF file')
    ap.add_argument('cat', help='Binary language catalog file')
    ap.add_argument('bin', help='Firmware BIN file')
    args = ap.parse_args()

    # get the language table position
    elf = elftools.elf.elffile.ELFFile(open(args.elf, "rb"))
    symtab = elf.get_section_by_name('.symtab')
    lang_table_sym = symtab.get_symbol_by_name('_SEC_LANG')[0]
    lang_table_start = lang_table_sym.entry.st_value
    lang_table_size = lang_table_sym.entry.st_size

    # read the binary catalog
    cat = open(args.cat, "rb").read()
    if len(cat) > lang_table_size:
        fatal("language catalog too large")

    # patch the symbol
    with open(args.bin, "r+b") as fw:
        fw.seek(lang_table_start)
        fw.write(cat)

    return 0

if __name__ == '__main__':
    exit(main())
