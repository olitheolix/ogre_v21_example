/* MIT License

Copyright (c) 2016 Oliver Nagy

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef OGREDEMOQT_H
#define OGREDEMOQT_H
 
#include <QtWidgets/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>

#include <Ogre.h>
#include "OgreRoot.h"
#include "OgreOverlaySystem.h"
#include "OgreTextAreaOverlayElement.h"

 
// The Ogre window inherits from QWindow.
class OgreDemoQt : public QWindow, public Ogre::FrameListener {
  /* Declare the Q_OBJECT keyword to ensure Qt's intermediate compiler
  can do the necessary wireup between our class and the rest of Qt. */
  Q_OBJECT
 
 public:
  explicit OgreDemoQt(QWindow *parent = NULL);
  ~OgreDemoQt();
 
 public slots:
  bool eventFilter(QObject *target, QEvent *event);
 
 protected:
  /* Core variables for Ogre window */
  Ogre::Root                       *mRoot;
  Ogre::Camera                     *mCamera;
  Ogre::CompositorWorkspace        *mWorkspace;
  Ogre::RenderWindow               *mRenderWindow;
  Ogre::SceneManager               *mSceneManager;
 
  /* Overlays to show the shadow maps and screen text */
  Ogre::v1::OverlaySystem          *mOverlaySystem;
  Ogre::v1::Overlay                *mOverlayPSSM;
  Ogre::v1::TextAreaOverlayElement *mOverlayText;
  Ogre::v1::Overlay                *mOverlaySpotlights;
  Ogre::v1::TextAreaOverlayElement *mOverlayTextShadow;

  /* The lights and objects defined in the scene */
  std::vector<Ogre::SceneNode*>    mLightNodes;
  std::vector<Ogre::SceneNode*>    mSceneNodes;

  bool m_update_pending;
 
  bool event(QEvent *event);
  void addCubes(void);
  void addLights(void);
  void addPlane(void);
  void createCamera(void);
  void createScene(void);
  void createSceneManager(void);
  void createShadowMapOverlays(void);
  void createTextOverlay(void);
  void exposeEvent(QExposeEvent *event);
  int initialise(void);
  void initialiseQtWindow(void);
  void registerHlms(void);
  void render(void);
  void renderLater(void);
  void renderNow(void);
  void setResourceLocations(void);
 };
#endif // OGREDEMOQT_H
