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

#ifndef _Demo_OgreDemoSDL_H_
#define _Demo_OgreDemoSDL_H_

#include <SDL.h>
#include <vector>
#include "OgreRoot.h"
#include "OgreOverlaySystem.h"
#include "OgreTextAreaOverlayElement.h"


class OgreDemoSDL
{
protected:
  SDL_Window                       *mSdlWindow;

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

  /* Flag to indicate if we should shut down */
  bool                             mQuit;

  Ogre::TexturePtr		   mRTT;


  void addPlane(void);
  void addCubes(void);
  void addLights(void);
  void checkCloseWindow(void);
  void createCamera(void);
  void createScene(void);
  void createSceneManager(void);
  void createShadowMapOverlays(void);
  void createTextOverlay(void);
  void initialiseSDLWindow(void);
  void shutdown(void);
  int initialise(void);
  void registerHlms(void);
  void setResourceLocations(void);
  void updateScene(void);

public:
  OgreDemoSDL();
  virtual ~OgreDemoSDL();
  int start(void);
};

#endif
