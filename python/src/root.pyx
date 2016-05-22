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

cdef class Root:
    cdef _Root *ptr

    def __cinit__(self):
        self.ptr = NULL

    def __init__(self):
        pass

    def setup(self, str fname, int width, int height):
        cdef RenderSystem *rs
        cdef RenderSystemList rsl

        # Remove hard coded name.
        self.ptr = new _Root(<string>fname.encode('utf8'))
        
        rsl = self.ptr.getAvailableRenderers()
        rs = rsl[0]

        tmp = '{} x {}'.format(width, height)
        rs.setConfigOption(<string>b"Video Mode", <string>tmp.encode('utf8'))
        rs.setConfigOption(<string>b"Full Screen", <string>b"No")
        rs.setConfigOption(<string>b"VSync", <string>b"Yes")
        self.ptr.setRenderSystem(rs)
        self.ptr.initialise(False)

    def destroy(self):
        del self.ptr
        self.ptr = NULL

    def registerHlms(self, str base_dir):
        cdef ArchiveManager *am = ArchiveManager_getSingleton()

        # Load the shader files.
        cdef Archive *archiveLibrary = am.load(
            <string>(base_dir + '/Hlms/Common/GLSL').encode('utf8'),
            <string>b'FileSystem',
            True)

        cdef Archive *archivePbs = am.load(
            <string>(base_dir + '/Hlms/Pbs/GLSL').encode('utf8'),
            <string>b'FileSystem',
            True)

        cdef Archive *archiveUnlit = am.load(
            <string>(base_dir + '/Hlms/Unlit/GLSL').encode('utf8'),
            <string>b'FileSystem',
            True)

        # Still no idea.
        cdef ArchiveVec library
        library.push_back(archiveLibrary)

        # // Dedicated classes that have something to do with lightning computation
        # // on modern hardware using HLMS.
        cdef HlmsPbs *hlmsPbs = new HlmsPbs(archivePbs, &library)
        cdef HlmsUnlit *hlmsUnlit = new HlmsUnlit(archiveUnlit, &library)

        # // Register the HLMS thingies.
        cdef _HlmsManager *hm = self.ptr.getHlmsManager()
        hm.registerHlms(<_Hlms*?>hlmsPbs)
        hm.registerHlms(<_Hlms*?>hlmsUnlit)

    def createRenderWindow(
            self, str name, int width, int height, dict d_param):

        cdef NameValuePairList param

        for k, v in d_param.items():
            param[k.encode('utf8')] = v.encode('utf8')

        rw = RenderWindow()
        rw.ptr = self.ptr.createRenderWindow(
                name.encode('utf8'),
                width,
                height,
                False,
                &param)
        rw.ptr.setVisible(True)
        return rw

    def createSceneManager(self, int scenetype):
        scene = SceneManager()

        cdef int numThreads = 1
        cdef InstancingThreadedCullingMethod cm

        if numThreads > 1:
            cm = INSTANCING_CULLING_THREADED
        else:
            cm = INSTANCING_CULLING_SINGLETHREAD
        # fixme: must be Ogre::ST_GENERIC
        scene.ptr = self.ptr.createSceneManager(
            ST_GENERIC, numThreads, cm, <string>"foo")
        return scene

    def renderOneFrame(self):
        messagePump()
        self.ptr.renderOneFrame()

    def getCompositorManager2(self):
        cm = CompositorManager2()
        cm.ptr = self.ptr.getCompositorManager2()
        return cm

    def getHlmsManager(self):
        hm = HlmsManager()
        hm.ptr = self.ptr.getHlmsManager()
        return hm

    def getFrameStats(self):
        fs = FrameStats()
        fs.ptr[0] = (self.ptr.getFrameStats())[0]
        return fs
