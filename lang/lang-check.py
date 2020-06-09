#!/usr/bin/env python3
#
# Version 1.0.1
#
#############################################################################
# Change log:
# 9 June 2020, 3d-gussner, Added version and Change log
# 9 June 2020, 3d-gussner, Wrap text to 20 char and rows
# 9 June 2020, 3d-gussner, colored output
#############################################################################
#
"""Check lang files."""
from argparse import ArgumentParser
from traceback import print_exc
from sys import stderr
import textwrap

red = lambda text: '\033[0;31m' + text + '\033[0m'
green = lambda text: '\033[0;32m' + text + '\033[0m'
yellow = lambda text: '\033[0;33m' + text + '\033[0m'

def parse_txt(lang, no_warning):
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
            source = src.readline()
            translation = src.readline()[:-1]
#Wrap text to 20 chars and rows
            wrapper = textwrap.TextWrapper(width=20)
            #wrap original/source
            rows_count_source = 0
            for line in wrapper.wrap(source.strip('"')):
                rows_count_source += 1
            #wrap translation
            rows_count_translation = 0
            for line in wrapper.wrap(translation.strip('"')):
                rows_count_translation += 1
#End wrap text

#Check if columns and rows are defined
            cols = None
            rows = None
            for item in comment[1:]:
                key, val = item.split('=')
                if key == 'c':
                    cols = int(val)
                elif key == 'r':
                    rows = int(val)
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

            if rows_count_translation > rows_count_source and rows_count_translation > rows:
                print(red("[E]: Text %s is longer then definiton on line %d rows diff=%d\n" % (translation, lines, rows_count_translation-rows)))

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

    args = parser.parse_args()
    try:
        parse_txt(args.lang, args.no_warning)
        return 0
    except Exception as exc:
        print_exc()
        parser.error("%s" % exc)
        return 1


if __name__ == "__main__":
    exit(main())
