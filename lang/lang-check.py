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
# 22 Apr. 2021, DRracer, add English source to output
#############################################################################
#
"""Check lang files."""
from argparse import ArgumentParser
from traceback import print_exc
from sys import stdout, stderr
import textwrap

def color_maybe(color_attr, text):
    if stdout.isatty():
        return '\033[0;' + str(color_attr) + 'm' + text + '\033[0m'
    else:
        return text

red = lambda text: color_maybe(31, text)
green = lambda text: color_maybe(32, text)
yellow = lambda text: color_maybe(33, text)

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
            #print (comment) #Debug
            source = src.readline()[:-1].strip('"')
            #print (source) #Debug
            translation = src.readline()[:-1].strip('"')
            #print (translation) #Debug
#Wrap text to 20 chars and rows
            wrapper = textwrap.TextWrapper(width=20)
            #wrap original/source
            rows_count_source = 0
            for line in wrapper.wrap(source):
                rows_count_source += 1
                #print (line) #Debug
            #wrap translation
            rows_count_translation = 0
            for line in wrapper.wrap(translation):
                rows_count_translation += 1
                #print (line) #Debug
#End wrap text

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

            if (rows_count_translation > rows_count_source and rows_count_translation > rows) or \
                    (rows == 1 and len(translation) > cols):
                print(red('[E]: Text "%s" is longer then definition on line %d (rows diff=%d)\n'
                          '[EN]:Text "%s" cols=%d rows=%d\n' % (translation, lines, rows_count_translation-rows, source, cols, rows)))


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
