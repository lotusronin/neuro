#!/bin/bash
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

exec cmake -G Ninja ../
exec ninja
