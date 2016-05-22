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

cdef class Light:
    cdef _Light *ptr

    def __cinit__(self):
        self.ptr = NULL
        
    def __init__(self):
        pass

    def setDirection(self, float x, float y, float z):
        cdef Vector3 vec = Vector3(x, y, z)
        self.ptr.setDirection(vec)

    def setType(self, int ltype):
        cdef LightTypes lt
        if ltype == 0:
            lt = LT_SPOTLIGHT
        else:
            lt = LT_DIRECTIONAL
        self.ptr.setType(lt)

    def setSpotlightRange(self, float a, float b):
        self.ptr.setSpotlightRange(Radian(b), Radian(a))

    def setAttenuationBasedOnRadius(self, float radius, float lumThresh):
        self.ptr.setAttenuationBasedOnRadius(radius, lumThresh)

    def setDiffuseColour(self, float r, float g, float b):
        self.ptr.setDiffuseColour(r, g, b)

    def setSpecularColour(self, float r, float g, float b):
        self.ptr.setSpecularColour(r, g, b)

    def setPowerScale(self, float power):
        self.ptr.setPowerScale(power)
