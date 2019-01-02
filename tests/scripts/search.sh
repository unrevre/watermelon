#!/bin/bash

[ $# -ne 2 ] && {
   echo "  usage: $BASH_SOURCE [depth] [input]";
   exit 1;
}

depth=$1
input=$2

while read fen; do
   [ $fen ] && ./bin/search $depth $fen || echo
done <<< "$(cat $input)"
