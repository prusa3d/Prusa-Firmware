import os
import sys

def info(msg):
    print(os.path.basename(sys.argv[0]) + ": " + msg)

def warn(msg):
    print(os.path.basename(sys.argv[0]) + ": " + msg, file=sys.stderr)

def fatal(msg):
    warn(msg)
    exit(1)
