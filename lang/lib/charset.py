# Mapping from LCD source to unicode characters
CUSTOM_CHARS = {
    '\x04': '🔃',
    '\xe4': 'µ',
    '\xdf': '°',
    '\xe1': 'ä',
    '\xe4': 'μ',
    '\xef': 'ö',
    '\xf5': 'ü',
}

def source_check(buf):
    valid_chars = set(CUSTOM_CHARS.values())
    valid_chars.add('\n')
    for c in buf:
        if (not c.isascii() or not c.isprintable()) and c not in valid_chars:
            return False
    return True

def source_to_unicode(buf):
    for src, dst in CUSTOM_CHARS.items():
        buf = buf.replace(src, dst)
    return buf

def unicode_to_source(buf):
    for dst, src in CUSTOM_CHARS.items():
        buf = buf.replace(src, dst)
    return buf
