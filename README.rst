===============================
Minimal Example for Ogre3D v2.1
===============================

The example is based on the official `ShadowMapDebugging` demo that ships with
Ogre. It shows an Ogre v2.1 scene and features several objects, lights, and
dynamic shadows.

The demo comes in three versions: SDL, Qt5, and Python (proof-of-concept only).

Each demo is coded as a single class in a single file to make it easier to
understand (I hope).

The demos focus on the Ogre aspect only. As such they lack keyboard/mouse
handling and use hard coded resource paths.

The following screenshot shows all three demos running simultaneously on my
laptop with an Nvidia GTX 750M graphics card.

.. image:: screenshots/ogre3d.jpg


Docker
======

The Dockerfile details all the steps to build Ogre and the demos. It uses
Anaconda to build them in virtual environments. This is not strictly necessary
but the virtual environemtns it provides keeps the Ogre and SDL files isolated
from other versions (possibly) installed on your system.

To build the container use

.. code-block:: bash

   docker build -t ogre:2.1 .

This will clone and build Ogre v2.1 as an Anaconda package. Then it will
compile the SDL, Qt5, and PyQt5 demos... this will take a while ;)

Note: the sole purpose of the Dockerfile is to provide repeatable build
instructions, not binaries for your host system (will probably not work).


Anaconda
========

If you are on (K)Ubuntu 16.04 (64Bit) and have Continuum's Anaconda
distribution installed then you should be able to use my pre-built Ogre
package. If not then you need to build them yourself first as per the
`Dockerfile` (see previous section).

Before you start you will need a C++ compiler (`apt-get install
build-essential` on Ubuntu).

Use the following commands to create an Ogre environment, compile the
demos, and run them:

.. code-block:: bash

   # Create an Anaconda environment for Ogre v2.1 and SDL.
   conda env create --file environment.yml
   source activate ogre

   # Create Ogre's plugin file.
   ./create_plugins_config.sh

   # Build and run the SDL demo.
   ./build_sdl.sh && sdl/build/sdl_demo

   # Build and run the Qt5 demo.
   ./build_qt.sh && qt/build/qt_demo

   # Build and run the PyQt5 demo.
   ./build_python && python python/pyqt_demo.py


Windows and OS X
================

The code will (presumably) require minor modification before it runs on these
systems - pull requests welcome.


License
=======

All code in this repository is licensed under the terms of the MIT license.
