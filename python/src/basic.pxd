# MIT License
#
# Copyright (c) 2016 Oliver Nagy
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

from libcpp.pair cimport pair
from libcpp.string cimport string
from libcpp.vector cimport vector

from string cimport *

cdef extern from "Ogre.h":
    ctypedef string String "Ogre::String"
    ctypedef float Real "Ogre::Real"
    ctypedef int uint32 "Ogre::uint32"
    ctypedef int uint16 "Ogre::uint16"
    ctypedef int uint8 "Ogre::uint8"

    cdef cppclass Vector3 "Ogre::Vector3":
        Vector3()
        Vector3(float, float ,float)
        float x
        float y
        float z
        
    cdef cppclass Radian "Ogre::Radian":
        Radian (Real r)

    cdef cppclass ColourValue "Ogre::ColourValue":
        ColourValue(float, float, float, float)


cdef extern from "OgreIdString.h":
    cdef cppclass IdString "Ogre::IdString":
        IdString()
        IdString(char *string)
        IdString(string &)

