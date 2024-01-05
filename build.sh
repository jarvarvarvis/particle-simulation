#!/bin/bash

build_type="$1"

mkdir -p build/

if [[ $build_type == "debug" ]]; then
    cd build/
    mkdir -p debug/
    cd debug/
    cmake ../.. -DCMAKE_BUILD_TYPE=Debug
    make
elif [[ $build_type == "release" || $build_type == "" ]]; then
    cd build/
    mkdir -p release/
    cd release/
    cmake ../..
    make
else
    echo "Unrecognized build type: $build_type!"
fi
