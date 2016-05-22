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

cdef class Camera:
    cdef _Camera *ptr

    def __cinit__(self):
        self.ptr = NULL

    def setAutoAspectRatio(self, bint autoratio):
        self.ptr.setAutoAspectRatio(autoratio)

    def setAspectRatio(self, float ratio):
        self.ptr.setAspectRatio(ratio)

    def lookAt(self, float x, float y, float z):
        self.ptr.lookAt(x, y, z)

    def setFarClipDistance(self, float farDist):
        self.ptr.setFarClipDistance(farDist)

    def setNearClipDistance(self, float nearDist):
        self.ptr.setNearClipDistance(nearDist)

    def setPosition(self, float x, float y, float z):
        self.ptr.setPosition(x, y, z)

    def getPosition(self):
        cdef Vector3 v = self.ptr.getPosition()
        return (v.x, v.y, v.z)

    def getDirection(self):
        cdef Vector3 v = self.ptr.getDirection()
        return (v.x, v.y, v.z)

    def getRight(self):
        cdef Vector3 v = self.ptr.getRight()
        return (v.x, v.y, v.z)

    def getUp(self):
        cdef Vector3 v = self.ptr.getUp()
        return (v.x, v.y, v.z)

    def setDirection(self, float x, float y, float z):
        self.ptr.setDirection(x, y, z)
