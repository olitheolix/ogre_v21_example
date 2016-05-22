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

cdef class V1TextAreaOverlayElement:
    cdef _V1TextAreaOverlayElement *ptr

    def __cinit__(self):
        self.ptr = NULL

    def createOverlayElement(self, str type_name, str instance_name,
                             bint isTemplate=False):
        cdef _V1OverlayManager *olm = V1OverlayManager_singletonPtr()

        self.ptr = <_V1TextAreaOverlayElement*?>olm.createOverlayElement(
            <string>type_name.encode('utf8'),
            <string>instance_name.encode('utf8'),
            isTemplate
        )
        return self

    def setColour(self, float r, float g, float b, float alpha):
        self.ptr.setColour(ColourValue(r, g, b, alpha))

    def setFontName(self, str font):
        self.ptr.setFontName("DebugFont".encode('utf8'))

    def setPosition(self, float left, float top):
        self.ptr.setPosition(left, top)

    def setCharHeight(self, float height):
        self.ptr.setCharHeight(height)

    def setCaption(self, str caption):
        self.ptr.setCaption(<string>caption.encode('utf8'))
