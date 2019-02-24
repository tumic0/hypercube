#!/bin/sh

HYPERCUBE=~/hypercube/hypercube-cli

if [ $# -ne 1 ]; then
	echo "Usage: $0 GRAPHDIR"
	exit 1
fi

if [ ! -d "$1" ]; then
	echo "$1: Not a directory"
	exit 1
fi

SRC="$1"
DST="$1.img"

mkdir -p "$DST"
parallel "$HYPERCUBE -ef 0 -vf 0 {}" ::: "$SRC/"*.graphml
mv "$SRC/"*.svg "$DST"
