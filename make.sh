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
        if cmake --build build; then
            echo "Build completed successfully."
            echo "Running Program..."
            ./build/Tiles
        else
            echo "Build failed. The program will not be run."
            exit 1
        fi
        ;;
    *)
        echo "Usage: ./make.sh {clean|build}"
        ;;
esac

