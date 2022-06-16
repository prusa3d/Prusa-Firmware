# Mapping from LCD source encoding to unicode characters
CUSTOM_CHARS = {
    '\x04': '🔃',
    '\xe4': 'µ',
    '\xdf': '°',
    '\xe1': 'ä',
    '\xe4': 'μ',
    '\xef': 'ö',
    '\xf5': 'ü',
}

# Charaters to be remapped prior to source-encoding transformation
# This transformation is applied to the translation prior to being converted to the final encoding,
# and maps UTF8 to UTF8. It replaces unavailable symbols in the translation to a close
# representation in the source encoding.
TRANS_CHARS = {
    'Ä': 'ä',
    'Å': 'A',
    'Ö': 'ö',
    'Ü': 'ü',
    'å': 'a',
    'æ': 'ä',
    'ø': 'ö',
    'ß': 'ss',
}


def _character_check(buf, valid_chars):
    for c in buf:
        if (not c.isascii() or not c.isprintable()) and c not in valid_chars:
            return c
    return None

def source_check(buf):
    valid_chars = set(CUSTOM_CHARS.values())
    valid_chars.add('\n')
    return _character_check(buf, valid_chars)

def translation_check(buf):
    valid_chars = set(CUSTOM_CHARS.keys())
    valid_chars.add('\n')
    return _character_check(buf, valid_chars)


def source_to_unicode(buf):
    for src, dst in CUSTOM_CHARS.items():
        buf = buf.replace(src, dst)
    return buf

def trans_replace(buf):
    for src, dst in TRANS_CHARS.items():
        buf = buf.replace(src, dst)
    return buf

def unicode_to_source(buf):
    buf = trans_replace(buf)
    for dst, src in CUSTOM_CHARS.items():
        buf = buf.replace(src, dst)
    return buf
