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
    CustomCharacter('🗘', 0x84, '\\xf3'),
    CustomCharacter('🗀', 0x85, '\\xdb'),
    CustomCharacter('»', 0x86, '>'),
    CustomCharacter('🕑', 0x87, '\\xe5'),
    CustomCharacter('↡', 0x88, '\\x7e'),
    CustomCharacter('✔', 0x89, '\\x7e'),

# from the default character set:
    CustomCharacter('ä', 0xe1),
    CustomCharacter('μ', 0xe4),
    CustomCharacter('ö', 0xef),
    CustomCharacter('ü', 0xf5),
]

def generateLineInTable(index, chars):
    pixels = chars[index]["PIXELS"].split(',')
    rows = []
    for i in range(8):
        rows.append(0)
        for j in range(5):
            rows[i] |= (1 << (5 - j - 1)) if pixels[j * 8 + i] == "0" else 0
    line = "{{"
    for r in rows:
        line += f"0x{r:02X}, "
    line += f"}}, '{fontTable[index].alternate}'}}, // '{fontTable[index].utf}'"
    return line

def generateFont():
    tree = ET.parse(os.path.join(sys.path[0], "Prusa.lcd"))
    root = tree.getroot()
    
    CharList = [Char.attrib for Char in root.iter("CHAR")]
    
    f = open(os.path.join(sys.path[0], "FontTable.h"), "w")
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
