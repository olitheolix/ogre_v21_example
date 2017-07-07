#!/bin/bash
set -e

# Directory with Qt demo.
dirname=qt/build

if [ "$CONDA_PREFIX" = "" ]; then
	echo "No Anaconda environment is active."
	exit 1
fi

# Tell pkgconfig where to find *.pc files for the Anaconda installed Ogre.
export PKG_CONFIG_PATH=$CONDA_ENV_PATH/lib/pkgconfig/

# Create a clean build directory. Then build the demo with CMake.
mkdir -p $dirname
rm -rf $dirname/*
cd $dirname
cmake -DCMAKE_MODULE_PATH=$CONDA_ENV_PATH/lib/OGRE/cmake ..
make -j
cd ../..
