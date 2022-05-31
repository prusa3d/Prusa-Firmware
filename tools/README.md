# Host debugging tools for Prusa MK3 firmware

## Tools

### ``dump_eeprom``

Dump the content of the entire EEPROM using the D3 command.
Requires ``printcore`` from [Pronterface].

### ``dump_sram``

Dump the content of the entire SRAM using the D2 command.
Requires ``printcore`` from [Pronterface].

### ``dump_crash``

Dump the content of the last crash dump on MK3+ printers using D21.
Requires ``printcore`` from [Pronterface].

### ``elf_mem_map``

Generate a symbol table map with decoded information starting directly from an ELF firmware with DWARF debugging information (which is the default using the stock board definition).

When used along with a memory dump obtained from the D2 g-code, show the value of each symbol which is within the address range of the dump.

When used with ``--map`` and a single elf file, generate a map consisting of memory location and source location for each statically-addressed variable.

With ``--qdirstat`` and a single elf file, generate a [qdirstat](https://github.com/shundhammer/qdirstat) compatible cache file which can be loaded to inspect memory utilization interactively in a treemap.

This assumes the running firmware generating the dump and the elf file are the same.
Requires Python3 and the [pyelftools](https://github.com/eliben/pyelftools) module.

### ``dump2bin``

Parse and decode a memory dump obtained from the D2/D21/D23 g-code into readable metadata and binary. The output binary is padded and extended to fit the original address range.

### ``xfimg2dump``

Extract a crash dump from an external flash image and output the same format produced by the D21 g-code.

### ``update_eeprom``

Given one EEPROM dump, convert the dump to update instructions that can be sent to a printer.

Given two EEPROM dumps, produces only the required instructions needed to update the contents from the first to the second. This is currently quite crude and assumes dumps are aligned (starting from the same address or same stride).

Optionally writes the instructions to the specified port (requires ``printcore`` from [Pronterface]).

### ``noreset``

Set the required TTY flags on the specified port to avoid reset-on-connect for *subsequent* requests (issuing this command might still cause the printer to reset).


[Pronterface]: https://github.com/kliment/Printrun
