import os
import sys
import ast

def info(msg):
    print(os.path.basename(sys.argv[0]) + ": " + msg)

def warn(msg):
    print(os.path.basename(sys.argv[0]) + ": " + msg, file=sys.stderr)

def fatal(msg):
    warn(msg)
    exit(1)


def load_map(path):
    fd = open(path, "r")

    # check the header
    if fd.readline() != 'OFFSET\tSIZE\tNAME\tID\tSTRING\n':
        fatal("invalid map file")

    # parse symbols
    syms = []
    for line in fd:
        line = line.rstrip('\n')
        offset, size, name, tr_id, data = line.split('\t', 4)
        data = ast.literal_eval(data)
        tr_id = int(tr_id) if len(tr_id) else None
        syms.append({'offset': int(offset, 16),
                     'size': int(size, 16),
                     'id': tr_id,
                     'name': name,
                     'data': data})
    return syms
