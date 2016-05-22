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

cdef class SceneManager:
    cdef _SceneManager *ptr

    def __cinit__(self):
        self.ptr = NULL
        
    def __init__(self):
        pass

    def createCamera(self, str name):
        cam = Camera()
        cam.ptr = self.ptr.createCamera(<string>name.encode('utf8'))
        return cam

    def createItem(self, str meshName):
        ent = Item()
        ent.ptr = self.ptr.createItem(<string>meshName.encode('utf8'))
        return ent

    def createLight(self):
        l = Light()
        l.ptr = self.ptr.createLight()
        return l

    def getRootSceneNode(self):
        sn = SceneNode()
        sn.ptr = self.ptr.getRootSceneNode()
        return sn

    def createPlane(self):
        cdef V1MeshManager *mm_v1 = V1MeshManager_singleton()
        cdef MeshManagerV2 *mm_v2 = MeshManagerV2_singleton()
        cdef MeshPtrV1 mp_v1 = mm_v1.createPlane(
            <string>b"Plane v1",
            <string>(DEFAULT_RESOURCE_GROUP_NAME),
            Plane(Vector3(0, 1, 0), 1.0),
            50.0, 50.0, 1, 1, True, 1, 4.0, 4.0, Vector3(0, 0, 1),
            HBU_STATIC, HBU_STATIC,
            True, True
        )

        cdef MeshPtrV2 mp_v2 = mm_v2.createManual("Plane", DEFAULT_RESOURCE_GROUP_NAME)

        # Import the plane into V2 format.
        mp_v2.get().importV1(mp_v1.get(), True, True, True)

        item = Item()
        item.ptr = self.ptr.createItem(mp_v2)
        return item

    def addRenderQueueListener(self, newListener):
        cdef _RenderQueueListener *rql

        # fixme: this is a mess
        if isinstance(newListener, RenderQueueListener):
            self.ptr.addRenderQueueListener(
                <_RenderQueueListener*?>((<RenderQueueListener?>newListener).ptr))
        elif isinstance(newListener, V1OverlaySystem):
            tmp = (<V1OverlaySystem?>newListener)
            rql = <_RenderQueueListener*?>(tmp.ptr)
            if rql == NULL:
                print('Invalid pointer!')
                assert False
            self.ptr.addRenderQueueListener(rql)
            pass
        else:
            assert False, "Unsupported listener"
