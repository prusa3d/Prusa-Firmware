import sys
import re
import enum
import struct
from . import avr


FILL_BYTE   = b'\0'      # used to fill memory gaps in the dump
DUMP_MAGIC  = 0x55525547 # XFLASH dump magic
DUMP_OFFSET = 0x3d000    # XFLASH dump offset
DUMP_SIZE   = 0x2300     # XFLASH dump size

class CrashReason(enum.IntEnum):
    MANUAL = 0
    STACK_ERROR = 1
    WATCHDOG = 2
    BAD_ISR = 3

class Dump():
    def __init__(self, typ, reason, regs, pc, sp, data, ranges):
        self.typ = typ
        self.reason = reason
        self.regs = regs
        self.pc = pc
        self.sp = sp
        self.data = data
        self.ranges = ranges


# expand the buffer identified by addr+data to fill the region start+size
def region_expand(addr, data, start, size):
    if start < addr:
        data = FILL_BYTE * (addr - start) + data
        addr = start
    end = start + size
    data_end = addr + len(data)
    if end > data_end:
        data += FILL_BYTE * (data_end - end)
    return addr, data


def merge_ranges(ranges):
    ranges = list(sorted(ranges, key=lambda x: x[0]))
    if len(ranges) < 2:
        return ranges

    ret = [ranges[0]]
    for cur in ranges[1:]:
        last = ret[-1]
        last_end = last[0] + last[1]
        if last_end < cur[0]:
            ret.append(cur)
        else:
            cur_end = cur[0] + cur[1]
            last = (last[0], max(last_end, cur_end) - last[0])
            ret[-1] = last
    return ret


def decode_dump(path):
    fd = open(path, 'r')
    if fd is None:
        return None

    buf_addr = None # starting address
    buf_data = None # data

    typ = None      # dump type
    reason = None   # crash reason
    regs = None     # registers present
    pc = None       # PC address
    sp = None       # SP address
    ranges = []     # dumped ranges

    in_dump = False
    for line in enumerate(fd):
        line = (line[0], line[1].rstrip())
        tokens = line[1].split(maxsplit=1)

        def line_error():
            print('malformed line {}: {}'.format(*line), file=sys.stderr)

        # handle metadata
        if not in_dump:
            if len(tokens) > 0 and tokens[0] in ['D2', 'D21', 'D23']:
                in_dump = True
                typ = tokens[0]
            continue
        else:
            if len(tokens) == 0:
                line_error()
                continue
            elif tokens[0] == 'ok':
                break
            elif tokens[0] == 'error:' and len(tokens) == 2:
                values = tokens[1].split(' ')
                if typ == 'D23' and len(values) >= 3:
                    reason = CrashReason(int(values[0], 0))
                    pc = int(values[1], 0)
                    sp = int(values[2], 0)
                else:
                    line_error()
                continue
            elif len(tokens) != 2 or not re.match(r'^[0-9a-fA-F]+$', tokens[0]):
                line_error()
                continue

        # decode hex data
        addr = int.from_bytes(bytes.fromhex(tokens[0]), 'big')
        data = bytes.fromhex(tokens[1])
        ranges.append((addr, len(data)))

        if buf_addr is None:
            buf_addr = addr
            buf_data = data
        else:
            # grow buffer as needed
            buf_addr, buf_data = region_expand(buf_addr, buf_data,
                                               addr, len(data))

            # replace new part
            rep_start = addr - buf_addr
            rep_end = rep_start + len(data)
            buf_data = buf_data[:rep_start] + data + buf_data[rep_end:]

    # merge continuous ranges
    ranges = merge_ranges(ranges)

    if typ == 'D2':
        # D2 doesn't guarantee registers to be present
        regs = len(ranges) > 0 and \
            ranges[0][0] == 0 and \
            ranges[0][1] >= avr.SRAM_START

        # fill to fit for easy loading
        buf_addr, buf_data = region_expand(
            buf_addr, buf_data, 0, avr.SRAM_START + avr.SRAM_SIZE)

    elif typ == 'D23':
        # check if the dump is complete
        if len(ranges) != 1 or ranges[0][0] != 0 or \
           ranges[0][1] != avr.SRAM_START + avr.SRAM_SIZE:
            print('error: incomplete D23 dump', file=sys.stderr)
            return None

        regs = True
        if reason is None:
            print('warning: no error line in D23', file=sys.stderr)

    elif typ == 'D21':
        if len(ranges) != 1 or len(buf_data) != (avr.SRAM_START + avr.SRAM_SIZE + 256):
            print('error: incomplete D21 dump', file=sys.stderr)
            return None

        # decode the header structure
        magic, regs_present, crash_reason, pc, sp = struct.unpack('<LBBLH', buf_data[0:12])
        if magic != DUMP_MAGIC:
            print('error: invalid dump header in D21', file=sys.stderr)
            return None

        regs = bool(regs_present)
        reason = CrashReason(crash_reason)

        # extract the data section
        buf_addr = 0
        buf_data = buf_data[256:]
        ranges[0] = (0, len(buf_data))

    return Dump(typ, reason, regs, pc, sp, buf_data, ranges)
