#!/usr/bin/env python3
import argparse
import bisect
import codecs
import polib
import regex
import os
import sys
import lib.charset as cs
from pathlib import Path

# Absolute path
BASE_DIR: Path = Path.absolute(Path(__file__).parent)
PO_DIR: Path = BASE_DIR / "po"

# Pathlib can't change the working directory yet
# The script is currently made to assume the working
# directory is ./lang/po
os.chdir(PO_DIR)

def line_warning(path, line, msg):
    print(f'{path}:{line}: {msg}', file=sys.stderr)

def line_error(path, line, msg):
    print(f'{path}:{line}: {msg}', file=sys.stderr)

def entry_warning_locs(entries):
    for msgid, data in entries:
        print('   text: ' + repr(msgid), file=sys.stderr)
        positions = ', '.join(map(lambda x: x[0] + ':' + str(x[1]), data['occurrences']))
        print('     in: ' + positions, file=sys.stderr)

def entries_warning(entries, msg):
    print('warning: ' + msg, file=sys.stderr)
    entry_warning_locs(entries)

def entry_warning(entry, msg):
    entries_warning([entry], msg)


def newline_positions(source):
    lines = [-1]
    while True:
        idx = source.find('\n', lines[-1] + 1)
        if idx < 0:
            break
        lines.append(idx)
    if lines[-1] != len(source) - 1:
        lines.append(len(source) - 1)
    return lines[1:]

def index_to_line(index, lines):
    return bisect.bisect_left(lines, index) + 1


def extract_file(path, catalog, warn_skipped=False):
    source = open(path, encoding="utf-8").read()
    newlines = newline_positions(source)

    # match internationalized quoted strings
    RE_START = r'\b (_[iI]|ISTR) \s* \('
    RE_META = r'//// \s* ([^\n]*)$'

    RE_I = fr'''
        (?<!(?:/[/*]|^\s*\#) [^\n]*)  # not on a comment or preprocessor
        {RE_START}                    # $1 ref type _i( or ISTR(
        (?:
          \s*
          ("(?:[^"\\]|\\.)*")         # $2 quoted string (chunk)
          (?:\s* {RE_META} )?         # $3 inline metadata
        )+
        \s* \)                        # )
        (?:
          (?:[^\n] (?!{RE_START}))*   # anything except another entry
          {RE_META}                   # $5 final metadata
        )?
    '''

    r_ref_type = 1
    r_quoted_chunk = 2
    r_inline_data = 3
    r_eol_data = 5

    for m in regex.finditer(RE_I, source, regex.M|regex.X):
        # parse the text
        line = index_to_line(m.start(0), newlines)

        text = ""
        for block in m.captures(r_quoted_chunk):
            # remove quotes and unescape
            block = block[1:-1]
            block = codecs.decode(block, 'unicode-escape', 'strict')
            block = cs.source_to_unicode(block)
            text += block

        # check if text is non-empty
        if len(text) == 0:
            line_warning(path, line, 'empty source text, ignored')
            continue

        data = set()
        comments = set()
        for n in [r_inline_data, r_eol_data]:
            meta = m.group(n)
            if meta is not None:
                meta_parts = meta.split('//', 1)
                data.add(meta_parts[0].strip())
                if len(meta_parts) > 1:
                    comments.add(meta_parts[1].strip())

        # check if this message should be ignored
        ignored = False
        for meta in data:
            if regex.search(r'\bIGNORE\b', meta) is not None:
                ignored = True
                break
        if ignored:
            if warn_skipped:
                line_warning(path, line, 'skipping explicitly ignored translation')
            continue

        # extra message catalog name (if any)
        cat_name = set()
        for meta in data:
            sm = regex.search(r'\b(MSG_\w+)', meta)
            if sm is not None:
                cat_name.add(sm.group(1))

        # reference type annotation
        ref_type = 'def' if m.group(r_ref_type) == 'ISTR' else 'ref'
        if ref_type == 'def':
            # ISTR definition: lookup nearby assignment
            lineup_def = source[newlines[line-2]+1:m.end(r_ref_type)]
            sm = regex.search(r'\b PROGMEM_(\S+) \s*=\s* ISTR $', lineup_def, regex.M|regex.X)
            if sm is None:
                line_warning(path, line, 'ISTR not used in an assignment')
            elif sm.group(1) != 'I1':
                line_warning(path, line, 'ISTR not used with PROGMEM_I1')

        # append the translation to the catalog
        pos = [(path, line)]
        entry = catalog.get(text)
        if entry is None:
            catalog[text] = {'occurrences': set(pos),
                             'data': data,
                             'cat_name': cat_name,
                             'comments': comments,
                             'ref_type': set([ref_type])}
        else:
            entry['occurrences'] = entry['occurrences'].union(pos)
            entry['data'] = entry['data'].union(data)
            entry['cat_name'] = entry['cat_name'].union(cat_name)
            entry['comments'] = entry['comments'].union(comments)
            entry['ref_type'].add(ref_type)


def extract_refs(path, catalog):
    source = open(path, encoding="utf-8").read()
    newlines = newline_positions(source)

    # match message catalog references to add backrefs
    RE_CAT = r'''
        (?<!(?:/[/*]|^\s*\#) [^\n]*)          # not on a comment or preprocessor
        \b (?:_[TOR]) \s* \( \s* (\w+) \s* \) # $1 catalog name
    '''

    for m in regex.finditer(RE_CAT, source, regex.M|regex.X):
        line = index_to_line(m.start(0), newlines)
        pos = [(path, line)]
        cat_name = m.group(1)
        found = False
        defined = False
        for entry in catalog.values():
            if cat_name in entry['cat_name']:
                entry['occurrences'] = entry['occurrences'].union(pos)
                entry['ref_type'].add('ref')
                found = True
                if 'def' in entry['ref_type']:
                    defined = True
        if not found:
            line_error(path, line, f'{cat_name} not found')
        elif not defined:
            line_error(path, line, f'{cat_name} referenced but never defined')


def check_entries(catalog, warn_missing, warn_same_line):
    cat_entries = {}

    for entry in catalog.items():
        msgid, data = entry

        # ensure we have at least one name
        if len(data['cat_name']) == 0 and warn_missing:
            entry_warning(entry, 'missing MSG identifier')

        # ensure references are being defined
        if data['ref_type'] == set(['def']):
            if len(data['cat_name']) == 0:
                if warn_missing:
                    entry_warning(entry, 'entry defined, but never used')
            else:
                id_name = next(iter(data['cat_name']))
                entry_warning(entry, f'{id_name} defined, but never used')

        # check custom characters
        invalid_char = cs.source_check(msgid)
        if invalid_char is not None:
            entry_warning(entry, 'source contains unhandled custom character ' + repr(invalid_char))

        tokens = []
        for meta in data['data']:
            tokens.extend(regex.split(r'\s+', meta))
        seen_keys = set()
        for token in tokens:
            if len(token) == 0:
                continue

            # check metadata syntax
            if regex.match(r'[cr]=\d+$', token) is None and \
               regex.match(r'MSG_[A-Z_0-9]+$', token) is None:
                entry_warning(entry, 'bogus annotation: ' + repr(token))

            # check for repeated keys
            key = regex.match(r'([^=])+=', token)
            if key is not None:
                key_name = key.group(1)
                if key_name in seen_keys:
                    entry_warning(entry, 'repeated annotation: ' + repr(token))
                else:
                    seen_keys.add(key_name)

            # build the inverse catalog map
            if token.startswith('MSG_'):
                if token not in cat_entries:
                    cat_entries[token] = [entry]
                else:
                    cat_entries[token].append(entry)

    # ensure the same id is not used in multiple entries
    for cat_name, entries in cat_entries.items():
        if len(entries) > 1:
            entries_warning(entries, f'{cat_name} used in multiple translations')

    if warn_same_line:
        # build the inverse location map
        entry_locs = {}
        for entry in catalog.items():
            msgid, data = entry
            for loc in data['occurrences']:
                if loc not in entry_locs:
                    entry_locs[loc] = [loc]
                else:
                    entry_locs[loc].append(loc)

        # check for multiple translations on the same location
        for loc, entries in entry_locs.items():
            if len(entries) > 1:
                line_warning(loc[0], loc[1], f'line contains multiple translations')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('-o', dest='pot', required=True, help='PO template output file')
    ap.add_argument('--no-missing', action='store_true',
                    help='Do not warn about missing MSG entries')
    ap.add_argument('--warn-same-line', action='store_true',
                    help='Warn about multiple translations on the same line')
    ap.add_argument('--warn-skipped', action='store_true',
                    help='Warn about explicitly ignored translations')
    ap.add_argument('-s', '--sort', action='store_true',
                    help='Sort output catalog')
    ap.add_argument('file', nargs='+', help='Input files')
    args = ap.parse_args()

    # extract strings
    catalog = {}
    for path in args.file:
        extract_file(path, catalog, warn_skipped=args.warn_skipped)

    # process backreferences in a 2nd pass
    for path in args.file:
        extract_refs(path, catalog)

    # check the catalog entries
    check_entries(catalog, warn_missing=not args.no_missing, warn_same_line=args.warn_same_line)

    # write the output PO template
    po = polib.POFile()
    po.metadata = {
        'Language': 'en',
        'MIME-Version': '1.0',
        'Content-Type': 'text/plain; charset=utf-8',
        'Content-Transfer-Encoding': '8bit'}

    messages = catalog.keys()
    if args.sort:
        messages = sorted(messages)
    for msgid in messages:
        data = catalog[msgid]
        comment = ', '.join(data['data'])
        if len(data['comments']):
            comment += '\n' + '\n'.join(data['comments'])
        occurrences = data['occurrences']
        if args.sort:
            occurrences = list(sorted(occurrences))
        po.append(
            polib.POEntry(
                msgid=msgid,
                comment=comment,
                occurrences=occurrences))

    po.save(args.pot)
    return 0

if __name__ == '__main__':
    exit(main())
