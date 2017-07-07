#!/bin/bash
set -e

# Directory with SDL demo.
dirname=python

if [ "$CONDA_PREFIX" = "" ]; then
	echo "No Anaconda environment is active."
	exit 1
fi

# Build the Cython wrapper.
cd $dirname
make clean && make
cd ..
