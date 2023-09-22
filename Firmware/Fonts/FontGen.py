#!/usr/bin/env python3

import argparse
from traceback import print_exc
import xml.etree.ElementTree as ET
import os
import sys

class CustomCharacter:
    def __init__(self, utf, default, alternate = None):
        self.utf = utf
        self.default = default
        self.alternate = alternate

fontTable = [
# custom characters:
    CustomCharacter('🄷', 0x80, 'H'),
    CustomCharacter('°', 0x81, '\\xdf'),
    CustomCharacter('🌡', 0x82, 'h'),
    CustomCharacter('⬏', 0x83, '^'),
    CustomCharacter('🔃', 0x84, '\\xf3'),
    CustomCharacter('🗀', 0x85, '\\xdb'),
    CustomCharacter('»', 0x86, '>'),
    CustomCharacter('🕑', 0x87, '\\xe5'),
    CustomCharacter('⏬', 0x88, '\\x7e'),
    CustomCharacter('✔', 0x89, '\\x7e'),
    CustomCharacter('ă', 0x8a, 'a'),
    CustomCharacter('â', 0x8b, 'a'),
    CustomCharacter('î', 0x8c, 'i'),
    CustomCharacter('ș', 0x8d, 's'),
    CustomCharacter('ț', 0x8e, 't'),
    CustomCharacter('Î', 0x8f, 'I'),
    CustomCharacter('Ș', 0x90, 'S'),
    CustomCharacter('Ț', 0x91, 'T'),

# from the default character set:
    CustomCharacter('ä', 0xe1),
    CustomCharacter('µ', 0xe4),
    CustomCharacter('ö', 0xef),
    CustomCharacter('ü', 0xf5),
]

def generateLineInTable(index, chars):
    pixels = chars[index]["PIXELS"].split(',')
    
    # Generate the rows binary data
    rows = []
    for i in range(8):
        rows.append(0)
        for j in range(5):
            rows[i] |= (1 << (5 - j - 1)) if pixels[j * 8 + i] == "0" else 0
    
    # compress the rows data
    colByte = 0
    compressedRows = []
    for i in range(4):
        rowByte = ((rows[i * 2 + 1] >> 1) & 0xF) | (((rows[i * 2 + 0] >> 1) & 0xF) << 4)
        colByte |= (1 << i * 2 + 0) if (rows[i * 2 + 0] & 0x1) else 0
        colByte |= (1 << i * 2 + 1) if (rows[i * 2 + 1] & 0x1) else 0
        compressedRows.append(rowByte)

    line = f"{{0x{colByte:02X}, {{"
    for r in compressedRows:
        line += f"0x{r:02X}, "
    line += f"}}, '{fontTable[index].alternate}'}}, // '{fontTable[index].utf}', \\x{fontTable[index].default:02X}"
    return line

def generateFont():
    tree = ET.parse(os.path.join(sys.path[0], "Prusa.lcd"))
    root = tree.getroot()
    
    CharList = [Char.attrib for Char in root.iter("CHAR")]
    
    f = open(os.path.join(sys.path[0], "FontTable.h"), "w", encoding='utf8')
    for x in range(len(fontTable)):
        if fontTable[x].default >= 0xE0:
            continue
        f.write(generateLineInTable(x, CharList) + '\n')
    f.close()

def utfToLCD(infile, outfile):
    return

def lcdToUTF(infile, outfile):
    return

def main():
    parser = argparse.ArgumentParser(description = "Layer between the unicode and the LCD")
    parser.add_argument("action", type = str, nargs = '?', default = "generateFont", 
        help = "What the script should do: (generateFont|utfToLCD|lcdToUTF)")
    parser.add_argument('infile', nargs = '?', type = argparse.FileType('r'), default = sys.stdin)
    parser.add_argument('outfile', nargs = '?', type = argparse.FileType('w'), default = sys.stdout)
    args = parser.parse_args()
    try:
        if args.action == "generateFont":
            generateFont()
        elif args.action == "utfToLCD":
            utfToLCD(args.infile, args.outfile)
        elif args.action == "lcdToUTF":
            lcdToUTF(args.infile, args.outfile)
        else:
            print("invalid action")
            return 1
        # parse_txt(args.lang, args.no_warning, args.warn_empty)
        return 0
    except Exception as exc:
        print_exc()
        parser.error("%s" % exc)
        return 1

if __name__ == "__main__":
    exit(main())
