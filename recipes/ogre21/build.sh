#!/bin/bash

mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_BUILD_TYPE=Release -DOGRE_BUILD_SAMPLES2=0 ..
make -j`nproc` install
