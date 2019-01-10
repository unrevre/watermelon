#!/bin/bash

[ $# -ne 3 ] && {
   echo "  usage: $BASH_SOURCE [unit] [depth] [input]";
   exit 1;
}

unit=$1
depth=$2
input=$3

while read fen; do
   [ -n "$fen" ] && $unit $depth "$fen" || echo
done <<< "$(cat $input)"
