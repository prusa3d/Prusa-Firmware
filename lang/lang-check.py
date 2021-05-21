#!/usr/bin/env python3
#
# Version 1.0.1
#
#############################################################################
# Change log:
#  7 May  2019, Ondrej Tuma, Initial
#  9 June 2020, 3d-gussner, Added version and Change log
#  9 June 2020, 3d-gussner, Wrap text to 20 char and rows
#  9 June 2020, 3d-gussner, colored output
#  2 Apr. 2021, 3d-gussner, Fix and improve text warp
# 22 Apr. 2021, DRracer   , add English source to output
# 23 Apr. 2021, wavexx    , improve
# 24 Apr. 2021, wavexx    , improve
# 26 Apr. 2021, 3d-gussner, add character ruler
#############################################################################
#
"""Check lang files."""
from argparse import ArgumentParser
from traceback import print_exc
from sys import stdout, stderr
import textwrap
import re

def color_maybe(color_attr, text):
    if stdout.isatty():
        return '\033[0;' + str(color_attr) + 'm' + text + '\033[0m'
    else:
        return text

red = lambda text: color_maybe(31, text)
green = lambda text: color_maybe(32, text)
yellow = lambda text: color_maybe(33, text)
cyan = lambda text: color_maybe(36, text)


def print_wrapped(wrapped_text, rows, cols):
    if type(wrapped_text) == str:
        wrapped_text = [wrapped_text]
    for r, line in enumerate(wrapped_text):
        r_ = str(r + 1).rjust(3)
        if r >= rows:
            r_ = red(r_)
        print((' {} |{:' + str(cols) + 's}|').format(r_, line))

def print_truncated(text, cols):
    if len(text) <= cols:
        prefix = text.ljust(cols)
        suffix = ''
    else:
        prefix = text[0:cols]
        suffix = red(text[cols:])
    print('   |' + prefix + '|' + suffix)

def print_ruler(spc, cols):
    print(' ' * spc + cyan(('₀₁₂₃₄₅₆₇₈₉'*4)[:cols]))

def print_source_translation(source, translation, wrapped_source, wrapped_translation, rows, cols):
    if rows == 1:
        print(' source text:')
        print_ruler(4, cols);
        print_truncated(source, cols)
        print(' translated text:')
        print_ruler(4, cols);
        print_truncated(translation, cols)
    else:
        print(' source text:')
        print_ruler(6, cols);
        print_wrapped(wrapped_source, rows, cols)
        print(' translated text:')
        print_ruler(6, cols);
        print_wrapped(wrapped_translation, rows, cols)
    print()

def highlight_trailing_white(text):
    if type(text) == str:
        return re.sub(r' $', '·', text)
    else:
        ret = text[:]
        ret[-1] = highlight_trailing_white(ret[-1])
        return ret

def wrap_text(text, cols):
    # wrap text
    ret = list(textwrap.TextWrapper(width=cols).wrap(text))
    if len(ret):
        # add back trailing whitespace
        ret[-1] += ' ' * (len(text) - len(text.rstrip()))
    return ret

def unescape(text):
    if '\\' not in text:
        return text
    return text.encode('ascii').decode('unicode_escape')

def ign_char_first(c):
    return c.isalnum() or c in {'%', '?'}

def ign_char_last(c):
    return c.isalnum() or c in {'.', "'"}


def parse_txt(lang, no_warning, warn_empty):
    """Parse txt file and check strings to display definition."""
    if lang == "en":
        file_path = "lang_en.txt"
    else:
        file_path = "lang_en_%s.txt" % lang

    print(green("Start %s lang-check" % lang))

    lines = 1
    with open(file_path) as src:
        while True:
            comment = src.readline().split(' ')
            #print (comment) #Debug

            #Check if columns and rows are defined
            cols = None
            rows = None
            for item in comment[1:]:
                key, val = item.split('=')
                if key == 'c':
                    cols = int(val)
                    #print ("c=",cols) #Debug
                elif key == 'r':
                    rows = int(val)
                    #print ("r=",rows) #Debug
                else:
                    raise RuntimeError(
                        "Unknown display definition %s on line %d" %
                        (' '.join(comment), lines))
            if cols is None and rows is None:
                if not no_warning:
                    print(yellow("[W]: No display definition on line %d" % lines))
                cols = len(translation)     # propably fullscreen
            if rows is None:
                rows = 1
            elif rows > 1 and cols != 20:
                print(yellow("[W]: Multiple rows with odd number of columns on line %d" % lines))

            #Wrap text to 20 chars and rows
            source = src.readline()[:-1].strip('"')
            #print (source) #Debug
            translation = src.readline()[:-1].strip('"')
            if translation == '\\x00':
                # crude hack to handle intentionally-empty translations
                translation = ''

            # handle backslash sequences
            source = unescape(source)
            translation = unescape(translation)

            #print (translation) #Debug
            wrapped_source = wrap_text(source, cols)
            rows_count_source = len(wrapped_source)
            wrapped_translation = wrap_text(translation, cols)
            rows_count_translation = len(wrapped_translation)

            # Check for potential errors in the definition
            if not no_warning:
                # Incorrect number of rows/cols on the definition
                if rows == 1 and (len(source) > cols or rows_count_source > rows):
                    print(yellow('[W]: Source text longer than %d cols as defined on line %d:' % (cols, lines)))
                    print_ruler(4, cols);
                    print_truncated(source, cols)
                    print()
                elif rows_count_source > rows:
                    print(yellow('[W]: Wrapped source text longer than %d rows as defined on line %d:' % (rows, lines)))
                    print_ruler(6, cols);
                    print_wrapped(wrapped_source, rows, cols)
                    print()

                # Missing translation
                if len(translation) == 0 and (warn_empty or rows > 1):
                    if rows == 1:
                        print(yellow("[W]: Empty translation for \"%s\" on line %d" % (source, lines)))
                    else:
                        print(yellow("[W]: Empty translation on line %d" % lines))
                        print_ruler(6, cols);
                        print_wrapped(wrapped_source, rows, cols)
                        print()


            # Check for translation lenght
            if (rows_count_translation > rows) or (rows == 1 and len(translation) > cols):
                print(red('[E]: Text is longer than definition on line %d: cols=%d rows=%d (rows diff=%d)'
                          % (lines, cols, rows, rows_count_translation-rows)))
                print_source_translation(source, translation,
                                         wrapped_source, wrapped_translation,
                                         rows, cols)

            # Different count of % sequences
            if source.count('%') != translation.count('%') and len(translation) > 0:
                print(red('[E]: Unequal count of %% escapes on line %d:' % (lines)))
                print_source_translation(source, translation,
                                         wrapped_source, wrapped_translation,
                                         rows, cols)

            # Different first/last character
            if not no_warning and len(source) > 0 and len(translation) > 0:
                source_end = source.rstrip()[-1]
                translation_end = translation.rstrip()[-1]
                start_diff = not (ign_char_first(source[0]) and ign_char_first(translation[0])) and source[0] != translation[0]
                end_diff = not (ign_char_last(source_end) and ign_char_last(translation_end)) and source_end != translation_end
                if start_diff or end_diff:
                    if start_diff:
                        print(yellow('[W]: Differing first punctuation character (%s => %s) on line %d:' % (source[0], translation[0], lines)))
                    if end_diff:
                        print(yellow('[W]: Differing last punctuation character (%s => %s) on line %d:' % (source[-1], translation[-1], lines)))
                    print_source_translation(source, translation,
                                             wrapped_source, wrapped_translation,
                                             rows, cols)

            # Short translation
            if not no_warning and len(source) > 0 and len(translation) > 0:
                if len(translation.rstrip()) < len(source.rstrip()) / 2:
                    print(yellow('[W]: Short translation on line %d:' % (lines)))
                    print_source_translation(source, translation,
                                             wrapped_source, wrapped_translation,
                                             rows, cols)

            # Incorrect trailing whitespace in translation
            if not no_warning and len(translation) > 0 and \
               (source.rstrip() == source or (rows == 1 and len(source) == cols)) and \
               translation.rstrip() != translation and \
               (rows > 1 or len(translation) != len(source)):
                print(yellow('[W]: Incorrect trailing whitespace for translation on line %d:' % (lines)))
                source = highlight_trailing_white(source)
                translation = highlight_trailing_white(translation)
                wrapped_translation = highlight_trailing_white(wrapped_translation)
                print_source_translation(source, translation,
                                         wrapped_source, wrapped_translation,
                                         rows, cols)

            if len(src.readline()) != 1:  # empty line
                break
            lines += 4
    print(green("End %s lang-check" % lang))


def main():
    """Main function."""
    parser = ArgumentParser(
        description=__doc__,
        usage="%(prog)s lang")
    parser.add_argument(
        "lang", nargs='?', default="en", type=str,
        help="Check lang file (en|cs|de|es|fr|nl|it|pl)")
    parser.add_argument(
        "--no-warning", action="store_true",
        help="Disable warnings")
    parser.add_argument(
        "--warn-empty", action="store_true",
        help="Warn about empty translations")

    args = parser.parse_args()
    try:
        parse_txt(args.lang, args.no_warning, args.warn_empty)
        return 0
    except Exception as exc:
        print_exc()
        parser.error("%s" % exc)
        return 1


if __name__ == "__main__":
    exit(main())
