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
from scene_manager cimport *
from render_target cimport *
from camera cimport *
from compositor_shadow_node cimport *


# fixme: move into separate file
cdef extern from "Compositor/OgreCompositorWorkspace.h":
    cdef cppclass _CompositorWorkspace "Ogre::CompositorWorkspace":
        _CompositorShadowNode* findShadowNode(IdString nodeDefName)

cdef extern from "Compositor/OgreCompositorManager2.h":
    cdef cppclass _CompositorManager2 "Ogre::CompositorManager2":
        _CompositorWorkspace* addWorkspace(
            _SceneManager*, RenderTarget*, _Camera *, IdString, bint)
