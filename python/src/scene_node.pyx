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

cdef class SceneNode:
    cdef _SceneNode *ptr

    def __cinit__(self):
        self.ptr = NULL
        
    def __init__(self):
        pass

    def createChildSceneNode(self):
        scene = SceneNode()
        scene.ptr = self.ptr.createChildSceneNode()
        return scene

    def attachObject(self, obj):
        cdef MovableObject *ptr
        if isinstance(obj, Item):
            ptr = <MovableObject*?>((<Item?>obj).ptr)
        elif isinstance(obj, Light):
            ptr = <MovableObject*?>((<Light?>obj).ptr)
        else:
            raise TypeError
        self.ptr.attachObject(ptr)

    def setPosition(self, float x, float y, float z):
        self.ptr.setPosition(x, y, z)

    def setOrientation(self, float w, float x, float y, float z):
        self.ptr.setOrientation(w, x, y, z)

    def setScale(self, float x, float y, float z):
        self.ptr.setScale(x, y, z)

    def roll(self, float val):
        self.ptr.roll(Radian(val))

    def yaw(self, float val):
        self.ptr.yaw(Radian(val))

    def createChildSceneNode(self):
        sn = SceneNode()
        sn.ptr = self.ptr.createChildSceneNode()
        return sn
