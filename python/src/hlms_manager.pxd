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

from hlms cimport *

cdef extern from "OgreHlmsCommon.h":
    enum HlmsTypes "Ogre::HlmsTypes":
       HLMS_LOW_LEVEL "Ogre::HLMS_LOW_LEVEL"
       HLMS_PBS "Ogre::HLMS_PBS"
       HLMS_TOON "Ogre::HLMS_TOON"
       HLMS_UNLIT "Ogre::HLMS_UNLIT"
       HLMS_USER0 "Ogre::HLMS_USER0"
       HLMS_USER1 "Ogre::HLMS_USER1"
       HLMS_USER2 "Ogre::HLMS_USER2"
       HLMS_USER3 "Ogre::HLMS_USER3"
       HLMS_MAX = 8


cdef extern from "OgreHlmsManager.h":
    cdef cppclass _HlmsManager "Ogre::HlmsManager":
        void registerHlms(_Hlms *)
        _Hlms* getHlms(HlmsTypes)
