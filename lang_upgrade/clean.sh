#!/bin/sh
#

echo "removing output files"
rm msgs*.txt
rm make_msgs.out
rm replace_*.out
rm ./source/*
rmdir ./source
echo "step0 finished... press key"
read