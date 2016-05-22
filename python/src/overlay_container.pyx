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

# fixme: file name must start with v1_

cdef class V1OverlayContainer:
    cdef _V1OverlayContainer *ptr

    def __cinit__(self):
        self.ptr = NULL

    def createOverlayElement(self, str type_name, str instance_name,
                             bint isTemplate=False):
        cdef _V1OverlayManager *olm = V1OverlayManager_singletonPtr()

        self.ptr = <_V1OverlayContainer*?>(olm.createOverlayElement(
            <string>type_name.encode('utf8'),
            <string>instance_name.encode('utf8'),
            isTemplate
        ))
        return self

    def addChild(self, V1TextAreaOverlayElement txt):
        self.ptr.addChild(<_V1OverlayElement*?>(txt.ptr))

    def setMetricsMode(self, int gmm):
        # fixme: use correct type
        cdef GuiMetricsMode gmm_tmp = GMM_RELATIVE_ASPECT_ADJUSTED
        self.ptr.setMetricsMode(gmm_tmp)

    def setPosition(self, float left, float top):
        self.ptr.setPosition(left, top)

    def setDimensions(self, float width, float height):
        self.ptr.setDimensions(width, height)

    def setMaterialName(self, str matName):
        self.ptr.setMaterialName(<string>matName.encode('utf8'))
