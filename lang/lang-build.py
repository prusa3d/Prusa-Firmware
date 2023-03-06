#!/usr/bin/env python3
from collections import defaultdict
import codecs
import argparse
import os
import polib
import struct
import sys

import lib.charset as cs
from lib.io import info, warn, fatal, load_map

FW_MAGIC = 0x4bb45aa5


def translation_ref(translation):
    cmt = translation.comment
    if cmt and cmt.startswith('MSG_'):
        return cmt.split(' ', 1)[0]
    else:
        return repr(translation.msgid)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--warn-unused', action='store_true',
                    help='Warn about unused translations')
    ap.add_argument('--show-coalesced', action='store_true',
                    help='List coalesced translations')
    ap.add_argument('map', help='Firmware symbol map file')
    ap.add_argument('po', help='PO file')
    ap.add_argument('out', help='output')
    args = ap.parse_args()

    # check arguments
    for path in [args.map, args.po]:
        if not os.path.isfile(path):
            fatal("{} does not exist or is not a regular file".format(args.po))

    # load the map file
    syms = load_map(args.map)
    fw_sig_data = None
    msgid_data = defaultdict(list)
    id_msgid = {}
    sym_cnt = 0
    for sym in syms:
        if sym['name'] == '_PRI_LANG_SIGNATURE':
            fw_sig_data = sym['data']
        else:
            # redo forward text transformation for transparent matching
            msgid = cs.source_to_unicode(codecs.decode(sym['data'], 'unicode_escape', 'strict'))
            msgid_data[msgid].append(sym)
            id_msgid[sym['id']] = msgid

            # update the max symbol count
            if sym_cnt <= sym['id']:
                sym_cnt = sym['id'] + 1

    if fw_sig_data is None:
        fatal('_PRI_LANG_SIGNATURE not found in map')

    # open translations
    po = polib.pofile(args.po)
    lang_code = po.metadata['Language']
    if not lang_code.isascii() or len(lang_code) != 2:
        fatal(f'unsupported language code {lang_code}')

    # build a catalog of all translations
    trans_table = {}
    for translation in po:
        if translation.obsolete:
            continue
        msgid = translation.msgid
        found = msgid in msgid_data
        if found:
            trans_table[msgid] = (translation, msgid_data[msgid])
        elif args.warn_unused:
            err = "{}:{}".format(args.po, translation.linenum)
            err += ": unused translation "
            err += translation_ref(translation)
            warn(err)

    for msgid, syms in msgid_data.items():
        if msgid not in trans_table:
            # warn about missing translations
            warn("untranslated text: " + repr(msgid))

    # write the binary catalog
    with open(args.out, "w+b") as fd:
        fixed_offset = 16+2*sym_cnt
        written_locs = {}

        # compute final data tables
        offsets = b''
        strings = b'\0'
        for i in range(sym_cnt):
            msgid = id_msgid.get(i)
            translation = trans_table.get(msgid)
            if translation is None or len(translation[0].msgstr) == 0 or translation[0].msgstr == msgid:
                # first slot reserved for untraslated/identical entries
                offsets += struct.pack("<H", fixed_offset)
            else:
                string_bin = cs.unicode_to_source(translation[0].msgstr)

                # check for invalid characters
                invalid_char = cs.translation_check(string_bin)
                if invalid_char is not None:
                    line = translation[0].linenum
                    warn(f'{args.po}:{line} contains unhandled character ' + repr(invalid_char))

                string_bin = string_bin.encode('raw_unicode_escape', 'ignore')
                string_off = written_locs.get(string_bin)
                offset = fixed_offset + len(strings)
                if string_off is not None:
                    # coalesce repeated strings
                    if args.show_coalesced:
                        info(f'coalescing {offset:04x}:{string_off:04x} {string_bin}')
                    offset = string_off
                else:
                    # allocate a new string
                    written_locs[string_bin] = offset
                    strings += string_bin + b'\0'
                offsets += struct.pack("<H", offset)

        # header
        size = 16 + len(offsets) + len(strings)
        header = struct.pack(
            "<IHHHHI",
            FW_MAGIC,
            size,
            sym_cnt,
            0, # no checksum yet
            (ord(lang_code[0]) << 8) + ord(lang_code[1]),
            fw_sig_data)

        fd.write(header)
        fd.write(offsets)
        fd.write(strings)

        # calculate and update the checksum
        cksum = 0
        fd.seek(0)
        for i in range(size):
            cksum += (ord(fd.read(1)) << (0 if i % 2 else 8))
            cksum &= 0xffff
        fd.seek(8)
        fd.write(struct.pack("<H", cksum))

    return 0


if __name__ == '__main__':
    exit(main())
