# MIT License
#
# Copyright (c) 2016 Oliver Nagy
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""
Reproduce the ShadowDebugMap demo in Python.

This demo uses the azOgre bindings (highly experimental at this stage).
"""

import os
import sys
import azOgre

from PyQt5.QtWidgets import QApplication, QWidget


class OgreWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        # Verify that <plugins.cfg> exitsts in current directory. Abort if not.
        try:
            open('plugins.cfg', 'r')
        except FileNotFoundError:
            print(
                'Error: could not find <plugins.cfg> in current directory.\n'
                'Run <create_config.sh> in root directory to create one.\n'
                'Then start this demo (from root directory) with '
                '$ python python/viewer.py\n\n'
                'Demo aborted\n'
            )
            sys.exit(1)

        # We cannot create the Ogre instance until Qt has actually created
        # the window. We therefore postpone all intialisations until the first
        # timer event triggers.
        self.root = None

        # Miscellaneous handles.
        self.scene = self.cam = self.compositor = self.workspace = None

        # Overlay images.
        self.mOverlayPSSM = self.mOverlaySpotlights = None

        # Overlay text.
        self.mOverlayText = self.mOverlayTextShadow = None

        # The actual target into which Ogre will render its scene.
        self.renderwindow = None

        # List of scene items/objects.
        self.myItems = []

        # List of lights.
        self.lights = []

        # Handle to the one plane in the scene.
        self.myPlane = None

        # We will use this value to set a periodic timer. Whenever it triggers
        # we tell Ogre to draw a frame (see `timerEvent` method for details).
        self.target_fps = 60

        # Continue with the initialisation once Qt has created the window.
        self.drawTimer = self.startTimer(0)

    def setupResources(self):
        """Tell Ogre where to find all the resources we will need.

        The "resources" for this demo are only meshes.
        """
        # Get the name of the directory this very file is in.
        pjoin = os.path.join
        cur_dir = os.path.dirname(os.path.abspath(__file__))

        # The Media directory is hard coded in this demo.
        media_dir = pjoin(cur_dir, '..', 'Media')

        # Convenience.
        arl = azOgre.ResourceGroupManager().getSingleton().addResourceLocation

        # Tell Ogre about the resources we want to load.
        arl(pjoin(media_dir, 'cube_d'), 'FileSystem', 'Essential')
        arl(pjoin(media_dir, 'boostercube'), 'FileSystem', 'Essential')
        arl(pjoin(media_dir, 'ogrecore'), 'FileSystem', 'Essential')
        arl(pjoin(media_dir, 'Compositors'), 'FileSystem', 'Popular')

        # Not sure what this does exactly, but it has to do with the
        # the new High Level Material System (HLMS).
        self.root.registerHlms(media_dir)
        rm = azOgre.ResourceGroupManager().getSingleton()

        # Tell Ogre to actually load all the resources we just specified.
        rm.initialiseAllResourceGroups()

    def setupCompositor(self):
        """Create a scene manager, setup a camera, and load a workspace.
        """
        # The scene manager.
        self.scene = self.root.createSceneManager(1)
        self.scene.addRenderQueueListener(self.overlaySystem)

        # A camera.
        self.cam = self.scene.createCamera('MainCamera')
        self.cam.setPosition(0, 5, 15)
        self.cam.lookAt(0, 0, 0)
        self.cam.setNearClipDistance(0.2)
        self.cam.setFarClipDistance(1000)
        self.cam.setAutoAspectRatio(True)

        # Not quite sure what the compositor does. However, the workspace name
        # specifies a file in the Media/Compositor directory which, in turn,
        # relates directly to how materials are rendered (including shadows).
        self.compositor = self.root.getCompositorManager2()
        self.workspace = self.compositor.addWorkspace(
            self.scene, self.renderwindow, self.cam,
            "ShadowMapDebuggingWorkspace", True)

    def setupScene(self):
        """Populate the scene with objects and lights.
        """
        # Create the plane.
        item = self.scene.createPlane()
        node = self.scene.getRootSceneNode().createChildSceneNode()
        node.setPosition(0, -1, 0)
        node.attachObject(item)
        del item, node

        # Crete the cubes.
        spacing = 2.5
        for ii in range(4):
            for jj in range(4):
                # fixme:has more arguments
                _item = self.scene.createItem("boostercube.mesh")
                _node = self.scene.getRootSceneNode().createChildSceneNode()

                self.myItems.append((_item, _node))
                _node.setPosition(
                    (ii - 1.5) * spacing,
                    2.0,
                    (jj - 1.5) * spacing
                )
                _node.setScale(0.65, 0.65, 0.65)
                _node.roll(ii * 4 + jj)
                _node.attachObject(_item)
                del jj, _item, _node
            del ii
        del spacing

        # Create a directional light.
        LT_SPOTLIGHT, LT_DIRECTIONAL = 0, 1
        light = self.scene.createLight()
        node = self.scene.getRootSceneNode().createChildSceneNode()
        node.attachObject(light)
        light.setPowerScale(LT_DIRECTIONAL)
        light.setType(1)
        light.setDirection(-1, -1, -1)
        self.lights.append((light, node))

        # Create a spot light.
        light = self.scene.createLight()
        node = self.scene.getRootSceneNode().createChildSceneNode()
        node.attachObject(light)
        light.setDiffuseColour(0.8, 0.4, 0.2)
        light.setSpecularColour(0.8, 0.4, 0.2)
        light.setPowerScale(3.1415)
        light.setType(LT_SPOTLIGHT)
        node.setPosition(-10.0, 10.0, 10.0)
        light.setDirection(1, -1, -1)
        light.setAttenuationBasedOnRadius(10.0, 0.01)
        self.lights.append((light, node))

        # Create another spot light (slightly different color).
        light = self.scene.createLight()
        node = self.scene.getRootSceneNode().createChildSceneNode()
        node.attachObject(light)
        light.setDiffuseColour(0.2, 0.4, 0.8)
        light.setSpecularColour(0.2, 0.4, 0.8)
        light.setPowerScale(3.1415)
        light.setType(LT_SPOTLIGHT)
        node.setPosition(10.0, 10.0, -10.0)
        light.setDirection(-1, -1, 1)
        light.setAttenuationBasedOnRadius(10.0, 0.01)
        self.lights.append((light, node))

    def resizeEvent(self, event):
        """Adapt the scene size whenever the window size changes.

        Qt will automatically call this method whenever the window size changes
        for whatever reason.
        """
        if self.renderwindow is None:
            return
        self.renderwindow.resize(self.width(), self.height())

    def initialiseQtWindow(self):
        """Connect Ogre to the Qt window.
        """
        # Qt Window handle. This is a pointer provided by the SIP tool.
        # In order to get the actual pointer address we need to cast it to
        # an Integer, and that value to a String because all entries in
        # Ogre's parameter structure must be strings.
        winID_str = str(int(self.winId()))
        param = {
            'currentGLContext': 'false',
            'externalWindowHandle': winID_str,
            'parentWindowHandle': winID_str,
            'gamma': 'true',
        }

        # Initialise the Ogre root object. Then setup the render window.
        self.root = azOgre.Root()
        self.root.setup('plugins.cfg', self.width(), self.height())
        self.renderwindow = self.root.createRenderWindow(
            'Qt Window', self.width(), self.height(), param)

        # Instantiate the overlay system. This _must_ happen before
        # the resources get loaded - no idea why.
        self.overlaySystem = azOgre.V1OverlaySystem()

        # Continue with intialisation, including scene setup.
        self.setupResources()
        self.setupCompositor()
        self.setupScene()
        self.createTextOverlay()
        self.createShadowMapOverlays()

    def timerEvent(self, event):
        """Update the scene.
        """
        # Acknowledget the timer.
        self.killTimer(event.timerId())

        # Specify when to render the next frame in Milliseconds.
        self.drawTimer = self.startTimer(1000 / self.target_fps)

        # Qt does not issue a timer event before the window was created.
        # Therefore, we can now initialise Ogre.
        if self.root is None:
            self.initialiseQtWindow()

        # Ask Ogre how long it took until we last called this function. In PyQt
        # this is somewhat pointless because it is directly determined by the
        # target frame rage (see self.target_fps). However, in order to closely
        # mimick the C++ demo we will do the same.
        frame_time = self.root.getFrameStats().getAvgTime()

        # Rotate the cubes.
        for idx, (item, node) in enumerate(self.myItems):
            node.yaw(idx * frame_time / 10000)

        # Render next frame.
        self.root.renderOneFrame()

        # Update the text on the screen.
        fps = 1000 / frame_time if frame_time > 1E-6 else 0
        text = ' Avg FPS: {}'.format(int(fps))
        text += (
            '\n\n'
            ' This demo is based on `ShadowMapDebugging`'
            ' from the Ogre Samples directory.\n'
            ' It shows dynamically rendered shadows, overlays'
            ' (red boxes at bottom) as well as text.'
        )
        self.mOverlayText.setCaption(text)
        self.mOverlayTextShadow.setCaption(text)

    def createTextOverlay(self):
        """Setup the overlay regions that will show the text.

        I do not really understand what goes on here - I merely wrapped the
        methods used in the C++ code :)
        """
        overlayManager = azOgre.V1OverlayManager()
        overlay = overlayManager.create('DebugText')
        panel = azOgre.V1OverlayContainer().createOverlayElement(
            'Panel', 'DebugPanel')

        mOverlayText = azOgre.V1TextAreaOverlayElement().createOverlayElement(
            'TextArea', 'DebugText')

        mOverlayText.setFontName('DebugFont')
        mOverlayText.setCharHeight(0.025)

        # The shadow text must have a name that starts with 0 (ie
        # "0DebugTextShadow") or otherwise it seems to get rendered _before_
        # the white text, instead of _after_. No idea why.
        mOverlayTextShadow = azOgre.V1TextAreaOverlayElement().createOverlayElement(
            'TextArea', '0DebugTextShadow')

        mOverlayTextShadow.setFontName('DebugFont')
        mOverlayTextShadow.setCharHeight(0.025)
        mOverlayTextShadow.setColour(0, 0, 0, 1)
        mOverlayTextShadow.setPosition(0.002, 0.002)

        panel.addChild(mOverlayTextShadow)
        panel.addChild(mOverlayText)
        overlay.add2D(panel)
        overlay.show()

        # Assign the overlay text to instance variables because we want to
        # update the FPS rate at every frame.
        self.mOverlayText = mOverlayText
        self.mOverlayTextShadow = mOverlayTextShadow

    def createShadowMapOverlays(self):
        """Setup the overlay regions that will show the text.

        I do not really understand what goes on here - I merely wrapped the
        methods used in the C++ code :)
        """
        # fixme: supply proper enum (Ogre::HLMS_UNLIT)
        hlmsUnlit = self.root.getHlmsManager().getHlms(0)

        macroblock = azOgre.HlmsMacroblock()

        # fixme: make this an attribute
        macroblock.depthCheck = False
        blendblock = azOgre.HlmsBlendblock()

        shadowNode = self.workspace.findShadowNode(
            'ShadowMapDebuggingShadowNode')

        for ii in range(5):
            name = 'depthShadow{}'.format(ii)
            # fixme: replace `dummy` with ParamVec once wired up in Cython.
            dummy = 0
            depthShadow = hlmsUnlit.createDatablock(
                name, name, macroblock, blendblock, dummy
            )

            # The following method does not exist in the original Ogre class.
            # The Cython wrapper provides is a shortcut to avoid wrapping
            # several auxiliary classes.
            shadowNode.setShadowNodeLocalTexture(ii, depthShadow)

        om = azOgre.V1OverlayManager()
        self.mOverlayPSSM = om.create('PSSM Overlays')
        self.mOverlaySpotlights = om.create('Spotlight overlays')

        for ii in range(5):
            # Create a panel
            panel = azOgre.V1OverlayContainer().createOverlayElement(
                'Panel', 'PanelName{}'.format(ii))
            # fixme: use proper enum (Ogre::v1::GMM_RELATIVE_ASPECT_ADJUSTED)
            panel.setMetricsMode(0)
            panel.setPosition(100 + ii * 1600, 10000 - 1600)
            panel.setDimensions(1500, 1500)
            panel.setMaterialName('depthShadow{}'.format(ii))

            if ii < 3:
                self.mOverlayPSSM.add2D(panel)
            else:
                self.mOverlaySpotlights.add2D(panel)

        self.mOverlayPSSM.show()
        self.mOverlaySpotlights.show()


def main():
    app = QApplication(sys.argv)

    w = OgreWidget()
    w.resize(640, 480)
    w.move(500, 300)
    w.setWindowTitle('Ogre 2.1: Shadow Map Demo (PyQt5)')
    w.show()

    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
