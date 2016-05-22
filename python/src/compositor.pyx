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

cdef class CompositorWorkspace:
    cdef _CompositorWorkspace *ptr

    def __cinit__(self):
        self.ptr = NULL

    def __init__(self):
        pass

    def findShadowNode(self, str nodeDefName):
        csn = CompositorShadowNode()
        csn.ptr = self.ptr.findShadowNode(IdString(<string>nodeDefName.encode('utf8')))
        return csn


cdef class CompositorManager2:
    cdef _CompositorManager2 *ptr

    def __cinit__(self):
        self.ptr = NULL

    def __init__(self):
        pass

    def addWorkspace(self, SceneManager sm, RenderWindow rw, Camera cam,
                         str name, bint enabled):
        cdef _CompositorWorkspace *cw
        cw = self.ptr.addWorkspace(
            sm.ptr, <RenderTarget*?>rw.ptr,
            cam.ptr,
            IdString(<string>name.encode('utf8')),
            enabled)

        pycw = CompositorWorkspace()
        pycw.ptr = cw
        return pycw
