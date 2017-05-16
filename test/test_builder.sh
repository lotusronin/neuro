#!/bin/bash
fileroot="test_$1"
srcf="$fileroot.nro"
conff="$fileroot.conf"
exptf="$fileroot.expected"

if [ ! -f $srcf ]; then
    if [ ! -f $conff ]; then
        if [ ! -f $exptf ]; then
            #make files
            touch $srcf
            touch $conff
            touch $exptf

            #print out conf file
            echo "conf = {" >> $conff
            echo "'src' : '$srcf'," >> $conff
            echo "'expected' : '$exptf'" >> $conff
            echo "}" >> $conff
        fi
    fi
fi
