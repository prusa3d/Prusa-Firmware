# Host debugging tools for Prusa MK3 firmware

Most of the tools require python 3 and assume an Unix environment.


## EEPROM analysis

### ``dump_eeprom``

Dump the content of the entire EEPROM using the D3 command.
Requires ``printcore`` from [Pronterface].

### ``update_eeprom``

Given one EEPROM dump, convert the dump to update instructions that can be sent to a printer.

Given two EEPROM dumps, produces only the required instructions needed to update the contents from the first to the second. This is currently quite crude and assumes dumps are aligned (starting from the same address or same stride).

Optionally writes the instructions to the specified port (requires ``printcore`` from [Pronterface]).


## Memory analysis

### ``dump_sram``

Dump the content of the entire SRAM using the D2 command.
Requires ``printcore`` from [Pronterface].

### ``elf_mem_map``

Generate a symbol table map with decoded information starting directly from an ELF firmware with DWARF debugging information (which is the default using the stock board definition).

When used along with a memory dump obtained from the D2 g-code, show the value of each symbol which is within the address range of the dump.

When used with ``--map`` and a single elf file, generate a map consisting of memory location and source location for each statically-addressed variable.

With ``--qdirstat`` and a single elf file, generate a [qdirstat](https://github.com/shundhammer/qdirstat) compatible cache file which can be loaded to inspect memory utilization interactively in a treemap.

This assumes the running firmware generating the dump and the elf file are the same.
Requires the [pyelftools](https://github.com/eliben/pyelftools) module.


## Crash dump handling

### ``dump_crash``

Dump the content of the last crash dump on MK3+ printers using D21.
Requires ``printcore`` from [Pronterface].

### ``dump2bin``

Parse and decode a memory dump obtained from the D2/D21/D23 g-code into readable metadata and binary. The output binary is padded and extended to fit the original address range.

### ``xfimg2dump``

Extract a crash dump from an external flash image and output the same format produced by the D21 g-code. Requires python 3.


## Serial handling

### ``noreset``

Set the required TTY flags on the specified port to avoid reset-on-connect for *subsequent* requests (issuing this command might still cause the printer to reset).


## Temperature analysis

### ``tml_decode``

Decode (or plot) the temperature model trace from a serial log file.

The TML trace needs to be enabled by issuing "M155 S1 C3" and "D70 S1" to the printer, generally followed by a temperature model calibration request "M310 A F0".

The parser is not strict, and will consume most serial logs with/without timestamps.

By default the decoded trace is written to the standard output as a tab-separated table. If `--plot` is used, produce a graph into the requested output file instead:

    ./tml_decode -p graph.png serial.log

When plotting the [Matplotlib](https://matplotlib.org/) module is required.


[Pronterface]: https://github.com/kliment/Printrun
