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

# fixme: remove redundant classes here
from basic cimport *


cdef extern from "OgreMeshManager.h":
    enum HardwareBufferUsage "Ogre::v1::HardwareBuffer":
        HBU_STATIC "Ogre::v1::HardwareBuffer::HBU_STATIC"
        HBU_DYNAMIC "Ogre::v1::HardwareBuffer::HBU_DYNAMIC"
        HBU_WRITE_ONLY "Ogre::v1::HardwareBuffer::HBU_WRITE_ONLY"
        HBU_DISCARDABLE "Ogre::v1::HardwareBuffer::HBU_DISCARDABLE"
        HBU_STATIC_WRITE_ONLY "Ogre::v1::HardwareBuffer::HBU_STATIC_WRITE_ONLY"
        HBU_DYNAMIC_WRITE_ONLY "Ogre::v1::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY"
        HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE "Ogre::v1::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE"

    cdef V1MeshManager *V1MeshManager_singleton "Ogre::v1::MeshManager::getSingletonPtr"()

    cdef cppclass Plane "Ogre::Plane":
        Plane(Vector3 &rkNormal, Real fConstant)

    cdef cppclass V1MeshManager "Ogre::v1::MeshManager":
        MeshPtrV1 createPlane(
                string &name,
                string &groupName,
                Plane &plane,
                Real width, Real height,
                int xsegments,
                int ysegments,
                bint normals,
                unsigned short numTexCoordSets,
                Real uTile,
                Real vTile,
                Vector3 &upVector,
                HardwareBufferUsage,
                HardwareBufferUsage,
                bint vertexShadowBuffer,
                bint indexShadowBuffer
            )
        
    cdef cppclass MeshPtrV1 "Ogre::v1::MeshPtr":
        MeshV1 *get()
        

cdef extern from "OgreMeshManager2.h":
    cdef MeshManagerV2 *MeshManagerV2_singleton "Ogre::MeshManager::getSingletonPtr"()

    cdef cppclass MeshManagerV2 "Ogre::MeshManager":
        MeshPtrV2 createManual(string &, string &)
        
    cdef cppclass MeshPtrV2 "Ogre::MeshPtr":
        MeshV2 *get()


cdef extern from "OgreMesh.h":
    cdef cppclass MeshV1 "Ogre::v1::Mesh":
        pass


cdef extern from "OgreMesh2.h":
    cdef cppclass MeshV2 "Ogre::Mesh":
        void importV1(MeshV1 *, bint, bint, bint)
