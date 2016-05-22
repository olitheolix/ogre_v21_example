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
from camera cimport *
from scene_node cimport *
from item cimport *
from mesh cimport *
from light cimport *
from render_queue_listener cimport *
from overlay_system cimport *


cdef extern from "OGRE/OgreSceneManager.h":
    enum SceneType "Ogre::SceneType":
        ST_GENERIC "Ogre::ST_GENERIC"

    enum InstancingThreadedCullingMethod "Ogre::InstancingThreadedCullingMethod":
        INSTANCING_CULLING_THREADED "Ogre::INSTANCING_CULLING_THREADED"
        INSTANCING_CULLING_SINGLETHREAD "Ogre::INSTANCING_CULLING_SINGLETHREAD"


    cdef cppclass _SceneManager "Ogre::SceneManager":
        _SceneManager(const string &instanceName)
        _Camera *createCamera(const string &name)
        _SceneNode* getRootSceneNode()
        _Item* createItem(const string &meshName)
        _Item* createItem(const MeshPtrV2 &pMesh)

        void setAmbientLight(const ColourValue &colour)
        _Light* createLight()
        void setShadowTechnique(ShadowTechnique)
        void setShadowTextureCasterMaterial(string)
        void setShadowTextureReceiverMaterial(string)
        void setShadowTextureSelfShadow(bint)
        void setShadowTextureSize(uint32)
        void addRenderQueueListener(_RenderQueueListener *)
