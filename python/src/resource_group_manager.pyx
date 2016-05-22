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

cdef class ResourceGroupManager:
    cdef _ResourceGroupManager *ptr

    def __cinit__(self):
        self.ptr = NULL

    def getSingleton(self):
        rgm = ResourceGroupManager()
        rgm.ptr = &ResourceGroupManager_getSingleton()
        return rgm

    def initialiseAllResourceGroups(self):
        self.ptr.initialiseAllResourceGroups()

    def initialiseResourceGroup(self, str name):
        self.ptr.initialiseResourceGroup(<string>name.encode('utf8'))

    def addResourceLocation(self, str archName, str typeName, str secName):
        self.ptr.addResourceLocation(
            <string>archName.encode('utf8'),
            <string>typeName.encode('utf8'),
            <string>secName.encode('utf8')
        )
