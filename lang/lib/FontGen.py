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
    CustomCharacter('Á', 24, 'A'),
    CustomCharacter('à', 17, 'a'),
    # CustomCharacter('À', 25, 'A'),
    CustomCharacter('â', 18, 'a'),
    # CustomCharacter('Â', 26, 'A'),
    CustomCharacter('Ä', 27, '\\xd1'),
    CustomCharacter('å', 20, 'a'),
    CustomCharacter('Å', 28, 'A'),
    CustomCharacter('æ', 21, '\\xd1'),
    CustomCharacter('ą', 22, 'a'),
    CustomCharacter('Ą', 30, 'A'),
    CustomCharacter('ă', 23, 'a'),
    CustomCharacter('Ă', 31, 'A'),
    CustomCharacter('ć', 32, 'c'),
    # CustomCharacter('Ć', 36, 'C'),
    # CustomCharacter('ç', 33, 'c'),
    # CustomCharacter('Ç', 37, 'C'),
    CustomCharacter('č', 34, 'c'),
    CustomCharacter('Č', 38, 'C'),
    CustomCharacter('ď', 40, 'd'),
    # CustomCharacter('Ď', 42, 'D'),
    # CustomCharacter('đ', 41, 'd'),
    # CustomCharacter('Đ', 43, 'D'),
    CustomCharacter('é', 44, 'e'),
    CustomCharacter('É', 52, 'E'),
    CustomCharacter('è', 45, 'e'),
    # CustomCharacter('È', 53, 'E'),
    CustomCharacter('ê', 46, 'e'),
    # CustomCharacter('Ê', 54, 'E'),
    CustomCharacter('ě', 47, 'e'),
    # CustomCharacter('Ě', 55, 'E'),
    CustomCharacter('ë', 48, 'e'),
    # CustomCharacter('Ë', 56, 'E'),
    CustomCharacter('ę', 49, 'e'),
    CustomCharacter('Ę', 57, 'E'),
    CustomCharacter('í', 60, 'i'),
    CustomCharacter('Í', 64, 'I'),
    # CustomCharacter('ì', 61, 'i'),
    # CustomCharacter('Ì', 65, 'I'),
    CustomCharacter('î', 62, 'i'),
    CustomCharacter('Î', 66, 'I'),
    CustomCharacter('ĺ', 68, 'l'),
    # CustomCharacter('Ĺ', 72, 'L'),
    CustomCharacter('ł', 69, 'l'),
    CustomCharacter('Ł', 73, 'L'),
    CustomCharacter('ľ', 70, 'l'),
    CustomCharacter('Ľ', 74, 'L'),
    CustomCharacter('ń', 76, 'n'),
    # CustomCharacter('Ń', 80, 'N'),
    CustomCharacter('ň', 77, 'n'),
    # CustomCharacter('Ň', 81, 'N'),
    # CustomCharacter('ñ', 78, 'n'),
    # CustomCharacter('Ñ', 82, 'N'),
    CustomCharacter('ó', 84, 'o'),
    CustomCharacter('Ó', 92, 'O'),
    CustomCharacter('ò', 85, 'o'),
    # CustomCharacter('Ò', 93, 'O'),
    CustomCharacter('ô', 86, 'o'),
    # CustomCharacter('Ô', 94, 'O'),
    # CustomCharacter('œ', 87, 'o'),
    # CustomCharacter('Œ', 95, 'O'),
    CustomCharacter('ø', 88, 'o'),
    CustomCharacter('Ø', 96, 'o'),
    CustomCharacter('Ö', 97, '\\xef'),
    CustomCharacter('ő', 90, 'o'),
    # CustomCharacter('Ő', 98, 'O'),
    # CustomCharacter('ŕ', 100, 'r'),
    # CustomCharacter('Ŕ', 104, 'R'),
    CustomCharacter('ř', 101, 'r'),
    CustomCharacter('Ř', 105, 'R'),
    CustomCharacter('ś', 108, 's'),
    CustomCharacter('Ś', 112, 'S'),
    CustomCharacter('š', 109, 's'),
    CustomCharacter('Š', 113, 'S'),
    CustomCharacter('ș', 110, 's'),
    CustomCharacter('Ș', 114, 'S'),
    CustomCharacter('ß', 111, '\\xe2'),
    # CustomCharacter('ẞ', 115, '\\xe2'),
    CustomCharacter('ť', 116, 't'),
    # CustomCharacter('Ť', 118, 'T'),
    CustomCharacter('ț', 117, 't'),
    # CustomCharacter('Ț', 119, 'T'),
    CustomCharacter('ú', 120, 'u'),
    # CustomCharacter('Ú', 128, 'U'),
    # CustomCharacter('ù', 121, 'u'),
    # CustomCharacter('Ù', 129, 'U'),
    # CustomCharacter('û', 122, 'u'),
    # CustomCharacter('Û', 130, 'U'),
    CustomCharacter('Ü', 133, 'U'),
    CustomCharacter('ů', 123, 'u'),
    # CustomCharacter('Ů', 131, 'U'),
    CustomCharacter('ű', 124, 'u'),
    # CustomCharacter('Ű', 132, 'U'),
    CustomCharacter('ý', 136, 'y'),
    CustomCharacter('Ý', 138, 'Y'),
    # CustomCharacter('ÿ', 137, 'y'),
    # CustomCharacter('Ÿ', 139, 'Y'),
    CustomCharacter('ź', 140, 'z'),
    # CustomCharacter('Ź', 144, 'Z'),
    CustomCharacter('ž', 141, 'z'),
    CustomCharacter('Ž', 145, 'Z'),
    CustomCharacter('ż', 142, 'z'),
    # CustomCharacter('Ż', 146, 'Z'),
    # CustomCharacter('¿', 254, ' '),
    # CustomCharacter('¡', 255, ' '),
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

INVERSE_CUSTOM_CHARS = {v: k for k, v in CUSTOM_CHARS.items()}

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
