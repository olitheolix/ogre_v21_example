# Build Ogre3D and demos.
#
# From this directory (where this <Dockerfile> is located) type:
#   >> docker build -t ogre .

FROM ubuntu:16.04
MAINTAINER Oliver Nagy <olitheolix@gmail.com>

# ----------------------------------------------------------------------
#                           Prerequisites
# ----------------------------------------------------------------------
RUN apt-get update && apt-get install -y \
    build-essential \
    bzip2 \
    freeglut3-dev \
    git \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libtool \
    libxaw7-dev \
    libxrandr-dev \
    libzzip-dev \
    mercurial \
    pkg-config \
    wget \
    && apt-get clean

# Clone Ogre3D.
RUN hg clone https://bitbucket.org/sinbad/ogre/ /src/ogre

# Download and extract SDL2 sources.
RUN cd /src \
    && wget https://libsdl.org/release/SDL2-2.0.4.tar.gz \
    && tar -xvzf SDL2-2.0.4.tar.gz

# Install Nvidia CG toolkit.
RUN wget http://developer.download.nvidia.com/cg/Cg_3.1/Cg-3.1_April2012_x86_64.deb \
    && dpkg -i Cg-3.1_April2012_x86_64.deb \
    && rm -f Cg-3.1_April2012_x86_64.deb

# Install Anaconda Python distribution.
ENV MINICONDA /miniconda
RUN wget https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh \
        -O miniconda.sh \
    && chmod a+x miniconda.sh \
    && ./miniconda.sh -b -p $MINICONDA \
    && $MINICONDA/bin/conda update conda -y \
    && rm -f miniconda.sh

# Prefetch conda packages for Ogre. This is not technically necessary but
# makes it easy to tune the build process later because the packages have
# already been downloaded.
RUN $MINICONDA/bin/conda install -y \
    anaconda-client \
    boost \
    cmake \
    conda-build \
    cython \
    freeimage \
    freetype \
    jpeg \
    libpng \
    libtiff \
    numpy \
    openssl \
    pkgconfig \
    setuptools \
    zlib

# Define some Anaconda related aliases for Bash.
RUN echo "alias activate='source "$MINICONDA"/bin/activate'" >> /root/.bashrc \
    && echo "alias deactivate='source "$MINICONDA"/bin/deactivate'" >> /root/.bashrc \
    && echo "alias conda=$MINICONDA/bin/conda" >> /root/.bashrc

# ----------------------------------------------------------------------
#         Build Ogre and the Demos inside Anaconda Environment
# ----------------------------------------------------------------------
# Create an Anaconda/Ogre environment.
RUN $MINICONDA/bin/conda create -y -n ogre \
    python=3.5 \
    anaconda-client \
    cython

# Install PyQt5 from Spyder (Continuum does not yet provide it).
RUN $MINICONDA/bin/conda install -y -n ogre -c spyder-ide pyqt5

# Add the Anaconda recipes for Ogre and SDL, then build them.
ADD recipes/ /recipes/
RUN cd /recipes && $MINICONDA/bin/conda build --python 3.5 sdl
RUN cd /recipes && $MINICONDA/bin/conda build --python 3.5 ogre

# Install the Ogre package into the Ogre environment.
RUN $MINICONDA/bin/conda install -y -n ogre ogre sdl --use-local

# Copy the demo files.
ADD ./ /demo/
WORKDIR /demo

# Build SDL/Qt/Python demo.
RUN /bin/bash -c "source $MINICONDA/bin/activate ogre; ./build_sdl.sh"
RUN /bin/bash -c "source $MINICONDA/bin/activate ogre; ./build_qt.sh"
RUN /bin/bash -c "source $MINICONDA/bin/activate ogre; ./build_python.sh"
