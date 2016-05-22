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

from basic cimport *

cdef extern from "OGRE/OgreLight.h":
    enum LightTypes "Ogre::Light::LightTypes":
        LT_POINT "Ogre::Light::LT_POINT"
        LT_DIRECTIONAL "Ogre::Light::LT_DIRECTIONAL"
        LT_SPOTLIGHT "Ogre::Light::LT_SPOTLIGHT"

    cdef cppclass _Light "Ogre::Light":
        void setDirection(const Vector3 &)
        void setType(LightTypes)
        void setSpotlightRange(const Radian &, const Radian &)
        void setPowerScale(float)
        void setDiffuseColour(Real, Real, Real)
        void setSpecularColour(Real, Real, Real)
        void setAttenuationBasedOnRadius(Real, Real)

