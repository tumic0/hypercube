#!/bin/sh

CLI=./hypercube-cli
DATA=data
IMAGES=examples

if [ ! -d $IMAGES ]; then
	mkdir $IMAGES
fi

# tree20
$CLI -s 320,240 -vs 10 -ef 16 -vf 14 -el 1 -it 8000 -ft 10 -cf 0.9 -ns 800 \
  -f eps -o $IMAGES/tree20.eps $DATA/tree20.txt
$CLI -s 320,240 -vs 10 -ef 16 -vf 14 -el 1 -it 8000 -ft 10 -cf 0.9 -ns 800 \
  -f svg -o $IMAGES/tree20.svg $DATA/tree20.txt
# grid
$CLI -s 320,240 -es 2 -vs 10 -ec \#00aaff -ef 0 -vf 14 -el 1 -it 1000 -ft 10 \
  -ns 800 -f eps -d -o $IMAGES/grid.eps $DATA/grid.txt
$CLI -s 320,240 -es 2 -vs 10 -ec \#00aaff -ef 0 -vf 14 -el 1 -it 1000 -ft 10 \
  -ns 800 -f svg -d -o $IMAGES/grid.svg $DATA/grid.txt
# g15
$CLI -s 200,200 -vs 10 -ec \#ffaa00 -ef 0 -vf 14 -el 0.9 -cr 10000 -it 2000 \
  -ft 10 -cf 0.9 -ns 600 -f eps -o $IMAGES/g15.eps $DATA/g15.txt
$CLI -s 200,200 -vs 10 -ec \#ffaa00 -ef 0 -vf 14 -el 0.9 -cr 10000 -it 2000 \
  -ft 10 -cf 0.9 -ns 600 -f svg -o $IMAGES/g15.svg $DATA/g15.txt
# 2h3
$CLI -s 200,150 -es 2 -vs 10 -ef 0 -vf 14 -c -cr 1000 -it 1000 -ft 10 -ns 800 \
  -f eps -o $IMAGES/2h3.eps $DATA/2h3.txt
$CLI -s 200,150 -es 2 -vs 10 -ef 0 -vf 14 -c -cr 1000 -it 1000 -ft 10 -ns 800 \
  -f svg -o $IMAGES/2h3.svg $DATA/2h3.txt
# h4
$CLI -s 200,150 -es 2 -vs 9 -ef 0 -vf 0 -c -cr 1000 -it 1000 -ft 10 -ns 600 \
  -f eps -o $IMAGES/h4.eps $DATA/h4.txt
$CLI -s 200,150 -es 2 -vs 9 -ef 0 -vf 0 -c -cr 1000 -it 1000 -ft 10 -ns 600 \
  -f svg -o $IMAGES/h4.svg $DATA/h4.txt
# strings
$CLI -s 180,135 -es 2 -vs 10 -c -ef 0 -vf 14 -d -el 0.4 -cr 1000 \
  -it 1000 -ft 10 -ns 600 -f eps -o $IMAGES/strings.eps $DATA/strings.txt
$CLI -s 180,135 -es 2 -vs 10 -c -ef 0 -vf 14 -d -el 0.4 -cr 1000 \
  -it 1000 -ft 10 -ns 600 -f svg -o $IMAGES/strings.svg $DATA/strings.txt
# g8a
$CLI -s 240,180 -es 2 -vs 11 -ec \#aaff00 -ef 0 -vf 14 -el 0.4 -cr 1000 \
  -it 1000 -ft 10 -ns 600 -e utf-8 -f eps -o $IMAGES/g8a.eps $DATA/g8a.txt
$CLI -s 240,180 -es 2 -vs 11 -ec \#aaff00 -ef 0 -vf 14 -el 0.4 -cr 1000 \
  -it 1000 -ft 10 -ns 600 -e utf-8 -f svg -o $IMAGES/g8a.svg $DATA/g8a.txt
#g8l
$CLI -s 240,180 -es 2 -vs 11 -ec \#aaff00 -ef 0 -vf 14 -el 0.4 -cr 1000 \
  -it 1000 -ft 10 -ns 600 -e utf-8 -f eps -o $IMAGES/g8l.eps $DATA/g8l.txt
$CLI -s 240,180 -es 2 -vs 11 -ec \#aaff00 -ef 0 -vf 14 -el 0.4 -cr 1000 \
  -it 1000 -ft 10 -ns 600 -e utf-8 -f svg -o $IMAGES/g8l.svg $DATA/g8l.txt
