#!/usr/bin/env bash
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

#exec cmake -G Ninja ../
#exec ninja
cmake -G Ninja ../
ninja
