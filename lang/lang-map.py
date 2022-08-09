#!/usr/bin/env python3
from collections import defaultdict
import argparse
import elftools.elf.elffile
import struct
import sys
import zlib

from lib.io import warn


def warn_sym(name, start, size, msg):
    warn(f'{name}[{start:x}+{size:x}]: {msg}')


def get_lang_symbols(elf, symtab):
    # fetch language markers
    pri_start = symtab.get_symbol_by_name("__loc_pri_start")[0].entry.st_value
    pri_end = symtab.get_symbol_by_name("__loc_pri_end")[0].entry.st_value
    text_data = elf.get_section_by_name('.text').data()

    # extract translatable symbols
    syms = []
    sym_id = 0
    for sym in sorted(symtab.iter_symbols(), key=lambda x: x.entry.st_value):
        sym_start = sym.entry.st_value
        sym_size = sym.entry.st_size
        sym_end = sym_start + sym_size
        if sym_start >= pri_start and sym_end <= pri_end and sym_size > 0:
            data = text_data[sym_start:sym_end]

            # perform basic checks on the language section
            if data[0] != 255 or data[1] != 255:
                warn_sym(sym.name, sym_start, sym_size, 'invalid location offset')
            if data[-1] != 0:
                warn_sym(sym.name, sym_start, sym_size, 'unterminated string')

            syms.append({'start': sym_start,
                         'size': sym_size,
                         'name': sym.name,
                         'id': sym_id,
                         'data': data[2:-1]})
            sym_id += 1

    return syms


def fw_signature(syms):
    # any id which is stable when the translatable string do not change would do, so build it out of
    # the firmware translation symbol table itself
    data = b''
    for sym in syms:
        data += struct.pack("<HHH", sym['start'], sym['size'], sym['id'])
        data += sym['name'].encode('ascii') + b'\0'
        data += sym['data'] + b'\0'
    return zlib.crc32(data)


def get_sig_sym(symtab, syms):
    pri_sym = symtab.get_symbol_by_name('_PRI_LANG_SIGNATURE')[0]
    pri_sym_data = fw_signature(syms)
    pri_sym = {'start': pri_sym.entry.st_value,
               'size': pri_sym.entry.st_size,
               'name': pri_sym.name,
               'id': '',
               'data': pri_sym_data}
    return pri_sym


def patch_binary(path, syms, pri_sym):
    fw = open(path, "r+b")

    # signature
    fw.seek(pri_sym['start'])
    fw.write(struct.pack("<I", pri_sym['data']))

    # string IDs
    for sym in syms:
        fw.seek(sym['start'])
        fw.write(struct.pack("<H", sym['id']))


def check_duplicate_data(syms):
    data_syms = defaultdict(list)
    for sym in syms:
        data_syms[sym['data']].append(sym)
    for data, sym_list in data_syms.items():
        if len(sym_list) > 1:
            sym_names = [x['name'] for x in sym_list]
            warn(f'symbols {sym_names} contain the same data: {data}')


def output_map(syms):
    print('OFFSET\tSIZE\tNAME\tID\tSTRING')
    for sym in syms:
        print('{:04x}\t{:04x}\t{}\t{}\t{}'.format(sym['start'], sym['size'], sym['name'], sym['id'], sym['data']))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('elf', help='Firmware ELF file')
    ap.add_argument('bin', nargs='?', help='Firmware BIN file')
    args = ap.parse_args()

    # extract translatable symbols
    elf = elftools.elf.elffile.ELFFile(open(args.elf, "rb"))
    symtab = elf.get_section_by_name('.symtab')
    syms = get_lang_symbols(elf, symtab)
    pri_sym = get_sig_sym(symtab, syms)

    # do one additional pass to check for symbols containing the same data
    check_duplicate_data(syms)

    # output the symbol table map
    output_map(syms + [pri_sym])

    # patch the symbols in the final binary
    if args.bin is not None:
        patch_binary(args.bin, syms, pri_sym)

    return 0

if __name__ == '__main__':
    exit(main())
