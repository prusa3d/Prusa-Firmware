#!/usr/bin/env python3

"""
Script updates the Firmware.pot file.

The script does the following:
1. Current Firmware.pot is backed up with a copy, Firmware.pot.bak
2. Runs lang-extract.py with all the correct arguments.
"""

import sys
import os
from pathlib import Path, PurePath, PurePosixPath
import shutil
import subprocess
from subprocess import CalledProcessError

# Constants
BASE_DIR: Path = Path.absolute(Path(__file__).parent)
PROJECT_DIR: Path = BASE_DIR.parent
PO_DIR: Path = BASE_DIR / "po"

# Regex pattern to search for source files
SEARCH_REGEX: str = "[a-zA-Z]*.[ch]*"

# Folders to search for messages
SEARCH_PATHS: list[str] = ["./Firmware", "./Firmware/mmu2"]


def main():
    # List of source files to extract messages from
    FILE_LIST: list[Path] = []

    # Start by creating a back-up of the current Firmware.pot
    shutil.copy(PO_DIR / "Firmware.pot", PO_DIR / "Firmware.pot.bak")

    # Get the relative prepend of Project directory relative to ./po directory
    # This should be something like '../../'
    # Note: Pathlib's relative_to() doesn't handle this case yet, so let's use os module
    rel_path = os.path.relpath(PROJECT_DIR, PO_DIR)

    # We want to search for the C/C++ files relative to the .po/ directory
    # Lets append to the search path an absolute path.
    for index, search_path in enumerate(SEARCH_PATHS.copy()):
        try:
            # Example: Converts ./Firmware to ../../Firmware
            SEARCH_PATHS[index] = PurePath(rel_path).joinpath(search_path)

            # Example: Convert ../../Firmware to ../../Firmware/[a-zA-Z]*.[ch]*
            SEARCH_PATHS[index] = PurePosixPath(SEARCH_PATHS[index]).joinpath(
                SEARCH_REGEX
            )
        except ValueError as error:
            print(error)

    # Extend the glob and append all found files into FILE_LIST
    for pattern in SEARCH_PATHS:
        for file in sorted(PO_DIR.glob(str(pattern))):
            FILE_LIST.append(file)

    # Convert the path to relative and use Posix format
    for index, absolute_path in enumerate(FILE_LIST.copy()):
        FILE_LIST[index] = PurePosixPath(absolute_path).relative_to(PO_DIR)

    # Run the lang-extract.py script
    SCRIPT_PATH = BASE_DIR.joinpath("lang-extract.py")
    try:
        subprocess.check_call(
            [
                "python",
                SCRIPT_PATH,
                "--no-missing",
                "-s",
                "-o",
                "./Firmware.pot",
                *FILE_LIST,
            ]
        )
    except CalledProcessError as error:
        print(error)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(-1)
