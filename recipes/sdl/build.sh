#!/bin/bash

ls
mkdir build
cd build
../configure --prefix=${PREFIX}
make -j`nproc` install
