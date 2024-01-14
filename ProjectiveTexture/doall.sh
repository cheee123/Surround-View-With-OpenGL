#! /bin/sh
rm -rf out/
cmake -S . -B out/build
./buildrun.sh
