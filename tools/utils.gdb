# -*- gdb-script -*-

define load_dump
  restore $arg0 binary 0x800000
  set $pc = (((unsigned long)$arg1) - 2) << 1
  set $sp = $arg2
  where
end

document load_dump
Load a crash dump, setup PC/SP and show the current backtrace
Usage: load_dump <file> <pc-addr> <sp-addr>
end


define sp_skip
  if $argc == 0
    set $shift = 3
  else
    set $shift = $arg0
  end
  set $new_pc = ((((unsigned long)*(uint8_t*)($sp+$shift+1)) << 16) + \
                 (((unsigned long)*(uint8_t*)($sp+$shift+2)) << 8) + \
                 (((unsigned long)*(uint8_t*)($sp+$shift+3)) << 0)) << 1
  set $new_sp = $sp+$shift+3
  select-frame 0
  set $saved_pc = $pc
  set $saved_sp = $sp
  set $pc = $new_pc
  set $sp = $new_sp
  where
end

document sp_skip
TODO
end


define sp_restore
  select-frame 0
  set $pc = $saved_pc
  set $sp = $saved_sp
  where
end

document sp_restore
TODO
end


define sp_test
  sp_skip $arg0
  set $pc = $saved_pc
  set $sp = $saved_sp
end

document sp_test
TODO
end


define sp_scan
  dont-repeat

  if $argc == 0
    set $sp_end = 0x802200
  else
    set $sp_end = $arg0
  end

  set $sp_pos = $sp
  while $sp_pos < ($sp_end-4)
    set $sp_off = $sp_pos - $sp
    printf "**** scanning %#x (+%u) ****\n", $sp_pos, $sp_off
    sp_test $sp_off
    set $sp_pos += 1
  end
end

document sp_scan
TODO
end
