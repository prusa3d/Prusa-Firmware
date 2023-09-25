#!/usr/bin/env python3

import xml.etree.ElementTree as ET
import os
import sys

class CustomCharacter:
    def __init__(self, utf, charListIndex, alternate = None):
        self.utf = utf
        self.charListIndex = charListIndex
        self.alternate = alternate

FONT_TABLE = [
    CustomCharacter('🄷', 0, 'H'),
    CustomCharacter('°', 1, '\\xdf'),
    CustomCharacter('🌡', 2, 'h'),
    CustomCharacter('⬏', 3, '^'),
    CustomCharacter('🔃', 4, '\\xf3'),
    CustomCharacter('🗀', 5, '\\xdb'),
    CustomCharacter('»', 6, '>'),
    CustomCharacter('🕑', 7, '\\xe5'),
    CustomCharacter('⏬', 8, '\\x7e'),
    CustomCharacter('✔', 9, '\\x7e'),
    CustomCharacter('á', 16, 'a'),
]

BUILTIN_CHARS = {
    '\x7e': '→',
    '\x7f': '←',
    '\xe1': 'ä',
    '\xe4': 'µ', #on keyboard AltGr+m it is \xC2\xB5
    '\xef': 'ö',
    '\xf5': 'ü',
    '\xff': '█',
}

# Mapping from LCD source encoding to unicode characters
CUSTOM_CHARS = {}
for index in range(len(FONT_TABLE)):
    CUSTOM_CHARS.update({chr(index + 0x80): FONT_TABLE[index].utf})
CUSTOM_CHARS.update(BUILTIN_CHARS)

def generateLineInTable(index, chars):
    pixels = chars[FONT_TABLE[index].charListIndex]["PIXELS"].split(',')
    
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
    line += f"}}, '{FONT_TABLE[index].alternate}'}}, // index=0x{index + 0x80:02X}, utf8='{FONT_TABLE[index].utf}'"
    return line

def generateFont():
    tree = ET.parse(os.path.join(sys.path[0], "Prusa.lcd"))
    root = tree.getroot()
    
    CharList = [Char.attrib for Char in root.iter("CHAR")]
    
    f = open(os.path.join(sys.path[0], "../../Firmware/FontTable.h"), "w", encoding='utf8')
    for index in range(len(FONT_TABLE)):
        f.write(generateLineInTable(index, CharList) + '\n')
    f.close()

def main():
    generateFont()

if __name__ == "__main__":
    exit(main())
