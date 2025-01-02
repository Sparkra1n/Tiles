#!/bin/bash

action=$1

case $action in
    clean)
        rm -rf ./build
        mkdir ./build
        echo "Build directory cleaned and recreated."
        ;;
    build)
        # Set the compiler to Clang
        export CC=clang
        export CXX=clang++

        cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
        cmake --build build
        echo "Build completed."
        echo "Running Program..."
        ./build/Tiles
        ;;
    *)
        echo "Usage: ./make.sh {clean|build}"
        ;;
esac
