#!/bin/bash
set -e

# Config
if [ -z "$CONFIG_OK" ]; then source config.sh; fi
if [ -z "$CONFIG_OK" -o "$CONFIG_OK" -eq 0 ]; then echo "$(tput setaf 1)Config NG!$(tput sgr0)" >&2; exit 1; fi

# Clean the temporary directory
TMPDIR=$(dirname "$0")/tmp
rm -rf "$TMPDIR"

# Remove internationalized firmware files
rm -f "${INTLHEX}"*.hex
