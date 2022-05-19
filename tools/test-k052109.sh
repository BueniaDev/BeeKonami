#!/bin/bash

if [[ ! -f "BeeKonami/video/K052109/libk052109.a" ]]; then
    echo "Please run this script from the directory where you built BeeKonami."
    exit 1
fi

if [[ $1 == "" ]]; then
    echo "Please specify a log file."
    exit 1
fi

file=$1
trimmed=$(basename $file _log.bin)

tests/video/k052109-test $1
ffmpeg -r 60 -f image2 -s 512x256 -i frame%d.png -vcodec libx264 -crf 25 -pix_fmt yuv420p $trimmed.mp4
rm frame*.png
