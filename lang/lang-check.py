#!/usr/bin/env python3
"""Check lang files."""
from argparse import ArgumentParser
from traceback import print_exc
from sys import stderr


def parse_txt(lang, no_warning):
    """Parse txt file and check strings to display definition."""
    if lang == "en":
        file_path = "lang_en.txt"
    else:
        file_path = "lang_en_%s.txt" % lang

    lines = 1
    with open(file_path) as src:
        while True:
            comment = src.readline().split(' ')
            src.readline()  # source
            translation = src.readline()[:-1]

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
                    print("[W]: No display definition on line %d" % lines)
                cols = len(translation)     # propably fullscreen
            if rows is None:
                rows = 1

            if len(translation)-2 > cols*rows:
                stderr.write(
                    "[E]: Text %s is longer then definiton on line %d\n" %
                    (translation, lines))
                stderr.flush()

            if len(src.readline()) != 1:  # empty line
                break
            lines += 4


def main():
    """Main function."""
    parser = ArgumentParser(
        description=__doc__,
        usage="$(prog)s lang")
    parser.add_argument(
        "lang", nargs='?', default="en", type=str,
        help="Check lang file (en|cs|de|es|fr|it|pl)")
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
