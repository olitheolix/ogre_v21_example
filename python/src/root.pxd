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

from libcpp.map cimport map

from basic cimport *
from string cimport *
from render_window cimport *
from scene_manager cimport *
from hlms cimport *
from hlms_pbs cimport *
from hlms_unlit cimport *
from hlms_manager cimport *
from compositor cimport *
from archive_manager cimport *
from frame_stats cimport *

from scene_manager cimport SceneType, InstancingThreadedCullingMethod


cdef extern from "OgreRoot.h":
    cdef cppclass RenderSystem "Ogre::RenderSystem":
        RenderSystem()
        void setConfigOption (const string &name, const string &value)
        const string& getName()

    ctypedef vector[RenderSystem*] RenderSystemList "Ogre::RenderSystemList"
    ctypedef map[string, string] NameValuePairList "Ogre::NameValuePairList"
    ctypedef int SceneTypeMask "Ogre::SceneTypeMask"

    cdef void messagePump "Ogre::WindowEventUtilities::messagePump"()

    cdef cppclass _Root "Ogre::Root":
        _Root(string)
        void setRenderSystem (RenderSystem*)
        _RenderWindow* initialise(bint)
        const RenderSystemList& getAvailableRenderers()
        _RenderWindow* createRenderWindow(string &, int, int, bint, NameValuePairList*)
        _SceneManager* createSceneManager(SceneType, size_t, InstancingThreadedCullingMethod,
                                         const string &)
        bint renderOneFrame()
        _HlmsManager* getHlmsManager()
        _CompositorManager2 *getCompositorManager2()
        const _FrameStats* getFrameStats()
