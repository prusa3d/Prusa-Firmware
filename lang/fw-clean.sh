#!/bin/sh
set -e

# Clean the temporary directory
TMPDIR=$(dirname "$0")/tmp
rm -rf "$TMPDIR"
