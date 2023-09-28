from .FontGen import CUSTOM_CHARS

# Charaters to be remapped prior to source-encoding transformation
# This transformation is applied to the translation prior to being converted to the final encoding,
# and maps UTF8 to UTF8. It replaces unavailable symbols in the translation to a close
# representation in the source encoding.
# sources
# https://en.wikipedia.org/wiki/Czech_orthography
# https://en.wikipedia.org/wiki/German_orthography
# https://en.wikipedia.org/wiki/French_orthography
# https://en.wikipedia.org/wiki/Spanish_orthography
# https://en.wikipedia.org/wiki/Italian_orthography
# https://en.wikipedia.org/wiki/Polish_alphabet
# https://en.wikipedia.org/wiki/Dutch_orthography
# https://en.wikipedia.org/wiki/Romanian_alphabet
# https://en.wikipedia.org/wiki/Hungarian_alphabet
# https://en.wikipedia.org/wiki/Gaj%27s_Latin_alphabet
# https://en.wikipedia.org/wiki/Slovak_orthography
# https://en.wikipedia.org/wiki/Swedish_alphabet
# https://en.wikipedia.org/wiki/Norwegian_orthography

TRANS_CHARS = {
    # 'á': 'a', #cz,fr,es,hu,sk
    # 'Á': 'A', #cz,fr,hu,sk
    # 'à': 'a', #fr,it
    'À': 'à', #fr,it
    # 'â': 'a', #fr,ro
    'Â': 'â', #ro
    # 'Ä': 'ä', #de,sv,no,sk
    # 'å': 'a', #sv,no
    # 'Å': 'A', #sv,no
    # 'æ': 'ä', #sv,no
    # 'ą': 'a', #pl
    # 'Ą': 'A', #pl
    # 'ă': 'a', #ro - a-breve
    # 'Ă': 'A', #ro - A-breve
    'ǎ': 'ă', #ro - a-caron
    'Ǎ': 'Ă', #ro - A-caron
    # 'ć': 'c', #pl,hr
    'Ć': 'ć', #pl,hr
    'ç': 'c', #fr,nl
    # 'č': 'c', #cz,hr,sk
    'Č': 'č', #cz,hr,sk
    # 'ď': 'd', #cz,sk
    'Ď': 'ď', #cz,sk
    'đ': 'd', #hr
    'Đ': 'D', #hr
    # 'é': 'e', #cz,fr,es,it,nl,hu,sk
    # 'É': 'E', #cz,fr,it,hu,sk
    # 'è': 'e', #fr,it,nl
    'È': 'è', #fr,it
    'ê': 'e', #fr,nl
    # 'ě': 'e', #cz
    'Ě': 'ě', #cz
    'ë': 'e', #fr
    # 'ę': 'e', #pl
    'Ę': 'ę', #pl
    # 'í': 'i', #cz,es,it,sk
    'Í': 'í', #cz,it,sk
    # 'î': 'i', #fr,ro
    # 'Î': 'I', #ro
    # 'ĺ': 'l', #sk
    'Ĺ': 'ĺ', #sk
    # 'ł': 'l', #pl
    # 'Ł': 'L', #pl
    # 'ľ': 'l', #sk
    # 'Ľ': 'L', #sk
    # 'ń': 'n', #pl
    'Ń': 'ń', #pl
    # 'ň': 'n', #cz,sk
    'Ň': 'ň', #cz,sk
    # 'ñ': 'n', #es,nl
    # 'ó': 'o', #cz,es,pl,hu,sk
    'Ó': 'ó', #cz,pl,hu,sk
    # 'ò': 'o', #it
    'Ò': 'ò', #it
    # 'ô': 'o', #fr,nl,sk
    'Ô': 'ô', #sk
    'œ': 'o', #fr
    # 'ø': 'ö', #sv,no
    'Ø': 'ø', #sv,no
    'Ö': 'ö', #de,sv,no,hu
    # 'ő': 'o', #hu
    'Ő': 'ő', #hu
    'ŕ': 'r', #sk
    'Ŕ': 'R', #sk
    # 'ř': 'r', #cz
    'Ř': 'ř', #cz
    # 'ś': 's', #pl
    'Ś': 'ś', #pl
    # 'š': 's', #cz,hr,sk
    'Š': 'š', #cz,hr,sk
    # 'ș': 's', #ro - s-comma
    'Ș': 'ș', #ro - S-comma
    'ş': 'ș', #ro - s-cedilla
    'Ş': 'ș', #ro - S-cedilla
    # 'ß': 'ss',#de
    # 'ť': 't', #cz,sk
    'Ť': 'ť', #cz,sk
    # 'ț': 't', #ro - t-comma
    # 'Ț': 'T', #ro - T-comma
    'ţ': 'ț', #ro - t-cedilla
    'Ţ': 'Ț', #ro - T-cedilla
    # 'ú': 'u', #cz,es,hu,sk
    'Ú': 'ú', #cz,hu,sk
    'ù': 'u', #it
    'Ù': 'U', #it
    'û': 'u', #fr
    'Ü': 'ü', #de,hu
    # 'ů': 'u', #cz
    'Ů': 'ů', #cz
    # 'ű': 'u', #hu
    'Ű': 'ű', #hu
    # 'ý': 'y', #cz,sk
    'Ý': 'ý', #cz,sk
    'ÿ': 'y', #fr
    # 'ź': 'z', #pl
    'Ź': 'ź', #pl
    # 'ž': 'z', #cz,hr,sk
    'Ž': 'ž', #cz,hr,sk
    # 'ż': 'z', #pl
    'Ż': 'ż', #pl
    '¿': '', #es
    '¡': '', #es
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
