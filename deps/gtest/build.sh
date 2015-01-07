#!/usr/bin/env sh

cd deps/gtest/src
if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake ..
make
cd ../../../..
cp deps/gtest/src/build/libgtest.a deps/gtest/lib
cp deps/gtest/src/build/libgtest_main.a deps/gtest/lib
cp -r deps/gtest/src/include/gtest deps/gtest/include
