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
    'Á': 'A', #cz,fr,hu,sk
    'à': 'a', #fr,it
    'À': 'A', #fr,it
    'â': 'a', #fr,ro
    'Â': 'A', #ro
    'Ä': 'ä', #de,sv,no,sk
    'å': 'a', #sv,no
    'Å': 'A', #sv,no
    'æ': 'ä', #sv,no
    'ą': 'a', #pl
    'Ą': 'A', #pl
    'ă': 'a', #ro
    'Ă': 'A', #ro
    'ć': 'c', #pl,hr
    'Ć': 'C', #pl,hr
    'ç': 'c', #fr,nl
    'č': 'c', #cz,hr,sk
    'Č': 'C', #cz,hr,sk
    'ď': 'd', #cz,sk
    'Ď': 'D', #cz,sk
    'đ': 'd', #hr
    'Đ': 'D', #hr
    'é': 'e', #cz,fr,es,it,nl,hu,sk
    'É': 'E', #cz,fr,it,hu,sk
    'è': 'e', #fr,it,nl
    'È': 'E', #fr,it
    'ê': 'e', #fr,nl
    'ě': 'e', #cz
    'ë': 'e', #fr
    'Ě': 'E', #cz
    'ę': 'e', #pl
    'Ę': 'E', #pl
    'í': 'i', #cz,es,it,sk
    'Í': 'I', #cz,it,sk
    'î': 'i', #fr,ro
    'Î': 'I', #ro
    'ĺ': 'l', #sk
    'Ĺ': 'L', #sk
    'ł': 'l', #pl
    'Ł': 'L', #pl
    'ľ': 'l', #sk
    'Ľ': 'L', #sk
    'ń': 'n', #pl
    'Ń': 'N', #pl
    'ň': 'n', #cz,sk
    'Ň': 'N', #cz,sk
    'ñ': 'n', #es,nl
    'ó': 'o', #cz,es,pl,hu,sk
    'Ó': 'O', #cz,pl,hu,sk
    'ò': 'o', #it
    'Ò': 'O', #it
    'ô': 'o', #fr,nl,sk
    'Ô': 'O', #sk
    'œ': 'o', #fr
    'ø': 'ö', #sv,no
    'Ø': 'ö', #sv,no
    'Ö': 'ö', #de,sv,no,hu
    'ő': 'o', #hu
    'Ő': 'O', #hu
    'ŕ': 'r', #sk
    'Ŕ': 'R', #sk
    'ř': 'r', #cz
    'Ř': 'R', #cz
    'ś': 's', #pl
    'Ś': 's', #pl
    'š': 's', #cz,hr,sk
    'Š': 'S', #cz,hr,sk
    'ș': 's', #ro - s-comma
    'Ș': 'S', #ro - S-comma
    'ş': 's', #ro - s-cedilla
    'Ş': 'S', #ro - S-cedilla
    'ß': 'ss',#de
    'ť': 't', #cz,sk
    'Ť': 'T', #cz,sk
    'ț': 't', #ro - t-comma
    'Ț': 'T', #ro - T-comma
    'ţ': 't', #ro - t-cedilla
    'Ţ': 'T', #ro - T-cedilla
    'ú': 'u', #cz,es,hu,sk
    'Ú': 'U', #cz,hu,sk
    'ù': 'u', #it
    'Ù': 'U', #it
    'û': 'u', #fr
    'Ü': 'ü', #de,hu
    'ů': 'u', #cz
    'Ů': 'U', #cz
    'ű': 'u', #hu
    'Ű': 'U', #hu
    'ý': 'y', #cz,sk
    'Ý': 'Y', #cz,sk
    'ÿ': 'y', #fr
    'ź': 'z', #pl
    'Ź': 'Z', #pl
    'ž': 'z', #cz,hr,sk
    'Ž': 'z', #cz,hr,sk
    'ż': 'z', #pl
    'Ż': 'Z', #pl
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
