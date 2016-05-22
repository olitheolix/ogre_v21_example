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

#include "OgreDemoQt.hpp"

#include "Compositor/OgreCompositorManager2.h"
#include "OgreItem.h"
#include "OgreMeshManager.h"
#include "OgreMeshManager2.h"
#include "OgreMesh2.h"
 
#include "OgreArchiveManager.h"

#include "OgreHlmsPbs.h"
#include "OgreHlmsUnlit.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsUnlitDatablock.h"

#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
#include "Compositor/OgreCompositorManager2.h"
#include "Compositor/OgreCompositorWorkspace.h"
#include "Compositor/OgreCompositorShadowNode.h"


#include "OgreFrameStats.h"


/* Note that we pass any supplied QWindow parent to the base QWindow
   class. This is necessary should we need to use our class within a
   container. */
OgreDemoQt::OgreDemoQt(QWindow *parent)
  : QWindow(parent),
    mRoot(NULL),
    mCamera(NULL),
    mWorkspace(NULL),
    mRenderWindow(NULL),
    mSceneManager(NULL),
    mOverlaySystem(NULL),
    mOverlayPSSM(NULL),
    mOverlayText(NULL),
    mOverlaySpotlights(NULL),
    mOverlayTextShadow(NULL),
    m_update_pending(false)
{
  // Necessary to ensure Qt calls our event filters (eg. window resize events).
  installEventFilter(this);
}
 
/* Destroy the Ogre3D scene upon destruction of the QWindow. */
OgreDemoQt::~OgreDemoQt() {delete mRoot;}
 
 
/*
Initialise the Qt window. This is based on the information from
http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Integrating+Ogre+into+QT5
*/
void OgreDemoQt::initialiseQtWindow(void)
{
  const Ogre::RenderSystemList& rsList = mRoot->getAvailableRenderers();
  Ogre::RenderSystem* rs = rsList[0];
 
  // Setting size and VSync on windows will solve a lot of problems
  QString dimensions = QString("%1 x %2").arg(this->width()).arg(this->height());
  rs->setConfigOption("Video Mode", dimensions.toStdString());
  rs->setConfigOption("Full Screen", "No");
  rs->setConfigOption("VSync", "Yes");
  mRoot->setRenderSystem(rs);
  mRoot->initialise(false);
 
  Ogre::NameValuePairList parameters;
  /* Flag within the parameters set so that Ogre3D initialises an
     OpenGL context on its own. */
  parameters["currentGLContext"] = Ogre::String("false");
 
  /* We need to supply the low level OS window handle to this QWindow
     so that Ogre3D knows where to draw the scene. Below is a
     cross-platform method on how to do this. If you set both options
     (externalWindowHandle and parentWindowHandle) this code will work
     with OpenGL and DirectX. */
  parameters["externalWindowHandle"] =
    Ogre::StringConverter::toString((unsigned long)(this->winId()));
  parameters["parentWindowHandle"] =
    Ogre::StringConverter::toString((unsigned long)(this->winId()));
 
  // fixme: unsure but has an effect on background.
  parameters.insert( std::make_pair("gamma", "true") );

  /* Note below that we supply the creation function for the Ogre3D
     window the width and height from the current QWindow object using
     the "this" pointer. */
  mRenderWindow = mRoot->createRenderWindow("QT Window",
                                      this->width(),
                                      this->height(),
                                      false,
                                      &parameters);
}

  
/* Our initialization function. Called by our renderNow() function
   once when the window is first exposed. */
int OgreDemoQt::initialise()
{
  // Ogre's plugin file (contains path to render libraries).
  char fname_config[] = "plugins.cfg";

  // Verify that the file exists. Abort with an error if does not.
  if (!std::ifstream(fname_config)) {
    printf("Error: could not find <plugins.cfg> in current directory.\n"
           "Run <create_config.sh> in root directory to create one.\n"
           "Then start this demo (from root directory) with "
           "$ ./qt/build/qt_shadowmap\n\n"
           "Demo aborted\n");
    return 1;
  }

  // Pick the first available renderer. WARNING: this only makes sense
  // if the plugin file only contains a single renderer (GL3+ in this case).
  mRoot = new Ogre::Root(Ogre::String(fname_config));

  // Qt window specific setup.
  initialiseQtWindow();

  // Initialise the overlay system.
  mOverlaySystem = OGRE_NEW Ogre::v1::OverlaySystem();

  // Tell Ogre which resource paths it should use.
  setResourceLocations();

  // Tell Ogre where to find the shaders etc.
  registerHlms();

  // Tell Ogre to actually go an parse the resource locations we
  // just specified.
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  // Create standard scene manager and specify basic shadow parameters.
  createSceneManager();

  // Create a camera.
  createCamera();

  // Specify the compositor. This appears to be an Ogre 2.1 feature and
  // is responsible for the shadows to "just work". The name
  // "ShadowMapDebuggingWorkspace" identifies a specific compositor,
  // specifically the one defined in the file
  // `Media/Compositors/ShadowMapDebugging.compositor`.
  mWorkspace = mRoot->getCompositorManager2()->
    addWorkspace( mSceneManager,
                  mRenderWindow,
                  mCamera,
                  "ShadowMapDebuggingWorkspace",
                  true );
  return 0;
}
 
//---------------------------------------------------------------------------
// Specify resource locations where Ogre can find meshes and textures.
//
// Note: these folders are usually specified in a `resources.cfg`
// file but parsing that file in C++ is... painful, even with the
// helper methods provided by Ogre. This demo uses hard coded paths
// for simplicity.
//---------------------------------------------------------------------------
void OgreDemoQt::setResourceLocations(void)
{
  Ogre::ResourceGroupManager::getSingleton().
    addResourceLocation("Media/cube_d/", "FileSystem", "Essential");

  Ogre::ResourceGroupManager::getSingleton().
    addResourceLocation("Media/boostercube/", "FileSystem", "Essential");

  Ogre::ResourceGroupManager::getSingleton().
    addResourceLocation("Media/ogrecore/", "FileSystem", "Essential");

  Ogre::ResourceGroupManager::getSingleton().
    addResourceLocation("Media/Compositors/", "FileSystem", "Popular");
}

//---------------------------------------------------------------------------
// Specify the resources for the High Level Material System (HLMS).
//
// Not quite sure how it works but it appears to be one of the
// ingredients to make shadows "just work". In particular, this
// method will tell Ogre the location of shader programs which take
// care of the rendering.
//---------------------------------------------------------------------------
void OgreDemoQt::registerHlms(void)
{
  // Directory with the materials for the cube (I think).
  Ogre::String dataFolder = "Media/";

  // Load the shader files.
  Ogre::Archive *archiveLibrary = Ogre::ArchiveManager::getSingletonPtr()->
    load(dataFolder + "Hlms/Common/GLSL", "FileSystem", true);
  Ogre::Archive *archivePbs = Ogre::ArchiveManager::getSingletonPtr()->
    load(dataFolder + "Hlms/Pbs/GLSL", "FileSystem", true );
  Ogre::Archive *archiveUnlit = Ogre::ArchiveManager::getSingletonPtr()->
    load(dataFolder + "Hlms/Unlit/GLSL", "FileSystem", true );

  // Still no idea.
  Ogre::ArchiveVec library;
  library.push_back( archiveLibrary );

  // Dedicated classes that have something to do with lightning computation
  // on modern hardware using HLMS.
  Ogre::HlmsPbs *hlmsPbs = OGRE_NEW Ogre::HlmsPbs( archivePbs, &library );
  Ogre::HlmsUnlit *hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit( archiveUnlit, &library );

  // Register the HLMS thingies.
  Ogre::Root::getSingleton().getHlmsManager()->registerHlms( hlmsPbs );
  Ogre::Root::getSingleton().getHlmsManager()->registerHlms( hlmsUnlit );
}

//---------------------------------------------------------------------------
// Create a (threaded) scene manager.
//
// The scene manager is the go-to entity for creating and adding
// lights, objects (items), nodes, and just about everything else in
// the scene.
//
// In conjunction with the HLMS it also (seems to) control the
// shadow parameters.
//---------------------------------------------------------------------------
void OgreDemoQt::createSceneManager(void)
{
  // Determine the number of cores (returns 0 if it could not do it).
  size_t numThreads = Ogre::PlatformInformation::getNumLogicalCores();
  if (numThreads < 1) numThreads = 1;

  // Create the Scene Manager, in this case a generic one
  mSceneManager = mRoot->
    createSceneManager( Ogre::ST_GENERIC,
                        numThreads,
                        Ogre::INSTANCING_CULLING_SINGLETHREAD,
                        "MySceneManager" );

  // No idea, but if you disable the following line the overlays vanish.
  mSceneManager->addRenderQueueListener( mOverlaySystem );

  // Set sane defaults for proper shadow mapping.
  mSceneManager->setShadowDirectionalLightExtrusionDistance( 500.0f );
  mSceneManager->setShadowFarDistance( 500.0f );
}


//---------------------------------------------------------------------------
// Add a static plane.
//
// It will serve as the "ground" to better show of the shadows.
// See the Doxygen documentation for what the parameters mean
// exactly. Also note that this uses the Plane API from Ogre 1.x
// since it appears to not have been ported to Ogre 2.x yet (?).
//---------------------------------------------------------------------------
void OgreDemoQt::addPlane(void)
{
  // Create a Plane with the legacy v1 interface, convert it to v2 format,
  // and add it to a scene node.
  Ogre::v1::MeshPtr planeMeshV1 = Ogre::v1::MeshManager::getSingleton().createPlane(
    "Plane v1",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
    Ogre::Plane( Ogre::Vector3::UNIT_Y, 1.0f ), 50.0f, 50.0f,
    1, 1, true, 1, 4.0f, 4.0f, Ogre::Vector3::UNIT_Z,
    Ogre::v1::HardwareBuffer::HBU_STATIC,
    Ogre::v1::HardwareBuffer::HBU_STATIC);

  Ogre::MeshPtr planeMesh = Ogre::MeshManager::getSingleton().
    createManual("Plane",
                 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

  // Import the plane into V2 format.
  planeMesh->importV1( planeMeshV1.get(), true, true, true );

  Ogre::Item *item = mSceneManager->createItem( planeMesh, Ogre::SCENE_DYNAMIC );
  Ogre::SceneNode *sceneNode = mSceneManager->getRootSceneNode(
      Ogre::SCENE_DYNAMIC )->createChildSceneNode( Ogre::SCENE_DYNAMIC );

  sceneNode->setPosition( 0, -1, 0 );
  sceneNode->attachObject( item );
}


//---------------------------------------------------------------------------
// Add a 4x4 grid of cube meshes.
//---------------------------------------------------------------------------
void OgreDemoQt::addCubes(void)
{
  // The spacing among the cubes.
  float spacing = 2.5f;

  for( int i=0; i<4; ++i ) {
    for( int j=0; j<4; ++j ) {
      // Load the mesh.
      Ogre::Item *item = mSceneManager->
        createItem("boostercube.mesh",
                   Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                   Ogre::SCENE_DYNAMIC);

      // Create a new node (we will attach the mesh to it shortly).
      Ogre::SceneNode *node = mSceneManager->getRootSceneNode( Ogre::SCENE_DYNAMIC )->
        createChildSceneNode( Ogre::SCENE_DYNAMIC );

      // Specify the position and orientation of the node.
      node->setPosition((i - 1.5f) * spacing, 2.0f,  (j - 1.5f) * spacing);
      node->setScale( 0.65f, 0.65f, 0.65f );
      node->roll( Ogre::Radian( (Ogre::Real) (i * 4 + j)) );

      // Assign our mesh to the node, then add the node to our container.
      node->attachObject( item );
      mSceneNodes.push_back(node);
    }
  }
}


//---------------------------------------------------------------------------
// Add three lights: one directional light and two spotlights. All
// lights have slightly different colours.
//---------------------------------------------------------------------------
void OgreDemoQt::addLights(void)
{
  // Define general purpose pointers for convenience.
  Ogre::Light *light = 0;
  Ogre::SceneNode *lightNode = 0;
  Ogre::SceneNode *rootNode = mSceneManager->getRootSceneNode();

  // Define a directional light and attach it to the node.
  light = mSceneManager->createLight();
  lightNode = rootNode->createChildSceneNode();
  lightNode->attachObject( light );
  light->setPowerScale( 1.0f );
  light->setType( Ogre::Light::LT_DIRECTIONAL );
  light->setDirection( Ogre::Vector3( -1, -1, -1 ).normalisedCopy() );
  mLightNodes.push_back(lightNode);

  // Define a spotlight and attach it to a new node.
  light = mSceneManager->createLight();
  lightNode = rootNode->createChildSceneNode();
  lightNode->attachObject( light );
  light->setDiffuseColour( 0.8f, 0.4f, 0.2f ); //Warm
  light->setSpecularColour( 0.8f, 0.4f, 0.2f );
  light->setPowerScale( Ogre::Math::PI );
  light->setType( Ogre::Light::LT_SPOTLIGHT );
  lightNode->setPosition( -10.0f, 10.0f, 10.0f );
  light->setDirection( Ogre::Vector3( 1, -1, -1 ).normalisedCopy() );
  light->setAttenuationBasedOnRadius( 10.0f, 0.01f );
  mLightNodes.push_back(lightNode);

  // Define another spotlight and attach it to a new node.
  light = mSceneManager->createLight();
  lightNode = rootNode->createChildSceneNode();
  lightNode->attachObject( light );
  light->setDiffuseColour( 0.2f, 0.4f, 0.8f ); //Cold
  light->setSpecularColour( 0.2f, 0.4f, 0.8f );
  light->setPowerScale( Ogre::Math::PI );
  light->setType( Ogre::Light::LT_SPOTLIGHT );
  lightNode->setPosition( 10.0f, 10.0f, -10.0f );
  light->setDirection( Ogre::Vector3( -1, -1, 1 ).normalisedCopy() );
  light->setAttenuationBasedOnRadius( 10.0f, 0.01f );
  mLightNodes.push_back(lightNode);
}


//---------------------------------------------------------------------------
// Create a scene with several objects and lights.
//---------------------------------------------------------------------------
void OgreDemoQt::createScene(void)
{
  // Populate the scene.
  addPlane();
  addCubes();
  addLights();

  // Add the overlay panels to display the shadow maps.
  createShadowMapOverlays();

  // Add the text overlays to display, yes, the text.
  createTextOverlay();
}


void OgreDemoQt::createCamera()
{
  // Create camera at specific location and with specific orientation.
  mCamera = mSceneManager->createCamera( "Main Camera" );
  mCamera->setPosition( Ogre::Vector3( 0, 5, 15 ) );
  mCamera->lookAt( Ogre::Vector3( 0, 0, 0 ) );

  // Specify the clip planes.
  mCamera->setNearClipDistance( 0.2f );
  mCamera->setFarClipDistance( 1000.0f );
  mCamera->setAutoAspectRatio( true );
}
 

void OgreDemoQt::render()
{
  /* How we tied in the render function for OGre3D with QWindow's
     render function. This is what gets call repeatedly. Note that we
     don't call this function directly; rather we use the renderNow()
     function to call this method as we don't want to render the Ogre3D
     scene unless everything is set up first. That is what renderNow()
     does.
 
     Theoretically you can have one function that does this check but
     from my experience it seems better to keep things separate and
     keep the render function as simple as possible. */
  Ogre::WindowEventUtilities::messagePump();
  mRoot->renderOneFrame();

  // Get average frame time in Milliseconds.
  double frameTime = mRoot->getFrameStats()->getAvgTime();

  // Rotate the cubes.
  for( uint i=0; i<mSceneNodes.size(); ++i )
    mSceneNodes[i]->yaw( Ogre::Radian(i * frameTime / 10000) );

  // Update overlay text to show the latest FPS value.
  Ogre::String text;
  text.reserve( 128 );
  text += " Avg FPS:\t";
  text += Ogre::StringConverter::toString( int(1000.0f / frameTime ));
  text += ("\n\n"
           " This demo is based on `ShadowMapDebugging`"
           " from the Ogre Samples directory.\n"
           " It shows dynamically rendered shadows, overlays"
           " (red boxes at bottom) as well as text.");

  // Set the overlay text and its shadow version.
  mOverlayText->setCaption( text );
  mOverlayTextShadow->setCaption( text );
}
 
void OgreDemoQt::renderLater()
{
  /* This function forces QWindow to keep rendering. Omitting this
     causes the renderNow() function to only get called when the window
     is resized, moved, etc. as opposed to all of the time; which is
     generally what we need. */
  if (!m_update_pending) {
    m_update_pending = true;
    QApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
  }
}
 
bool OgreDemoQt::event(QEvent *event)
{
  /* QWindow's "message pump". The base method that handles all
     QWindow events. As you will see there are other methods that
     actually process the keyboard/other events of Qt and the
     underlying OS.
 
     Note that we call the renderNow() function which checks to see if
     everything is initialized, etc. before calling the `render()`
     function. */
 
  switch (event->type()) {
  case QEvent::UpdateRequest:
    m_update_pending = false;
    renderNow();
    return true;
  default:
    return QWindow::event(event);
  }
}
 
/*
  Called after the QWindow is reopened or when the QWindow is first opened.
*/
void OgreDemoQt::exposeEvent(QExposeEvent *event)
{
  // Suppress compiler warning about unused `event`.
  Q_UNUSED(event);
 
  if (isExposed()) renderNow();
}
 
/* The renderNow() function calls the initialize() function when
   needed and if the QWindow is already initialized and prepped calls
   the render() method. */
void OgreDemoQt::renderNow()
{
  // Do nothing if the window is invisible.
  if (!isExposed()) return;

  // If this is the first frame to render then it is time to initialise Ogre
  // and create a scene with objects and lights.
  if (mRoot == NULL) {
    if (initialise() != 0) exit(1);
    createScene();
  }
 
  render();
  renderLater();
}
 
/* Our event filter; handles the resizing of the QWindow. When the
   size of the QWindow changes note the call to the Ogre3D window and
   camera. This keeps the Ogre3D scene looking correct.
*/
bool OgreDemoQt::eventFilter(QObject *target, QEvent *event)
{
  if (target == this)
    if (event->type() == QEvent::Resize)
      if (isExposed() && mRenderWindow != NULL)
        mRenderWindow->resize(this->width(), this->height());
  return false;
}

//---------------------------------------------------------------------------
// Render the text with shadows.
//
// The shadow effect is simple: first render the entire text in black, then
// render the same text again in white but with a tiny position offset.
//---------------------------------------------------------------------------
void OgreDemoQt::createTextOverlay(void)
{
  Ogre::v1::OverlayManager &overlayManager = Ogre::v1::OverlayManager::getSingleton();
  Ogre::v1::Overlay *overlay = overlayManager.create( "DebugText" );

  Ogre::v1::OverlayContainer *panel = static_cast<Ogre::v1::OverlayContainer*>(
      overlayManager.createOverlayElement("Panel", "DebugPanel")
  );
  mOverlayText = static_cast<Ogre::v1::TextAreaOverlayElement*>(
      overlayManager.createOverlayElement( "TextArea", "DebugText" )
  );
  mOverlayText->setFontName( "DebugFont" );
  mOverlayText->setCharHeight( 0.025f );

  // The shadow text must have a name that starts with 0 (ie
  // "0DebugTextShadow") or otherwise it seems to get rendered _before_
  // the white text instead of _after_. No idea why.
  mOverlayTextShadow= static_cast<Ogre::v1::TextAreaOverlayElement*>(
      overlayManager.createOverlayElement( "TextArea", "0DebugTextShadow" )
  );
  mOverlayTextShadow->setFontName( "DebugFont" );
  mOverlayTextShadow->setCharHeight( 0.025f );
  mOverlayTextShadow->setColour( Ogre::ColourValue::Black );
  mOverlayTextShadow->setPosition( 0.002f, 0.002f );

  panel->addChild( mOverlayTextShadow );
  panel->addChild( mOverlayText );
  overlay->add2D( panel );
  overlay->show();
}

//---------------------------------------------------------------------------
// This method is somehow responsible for drawing the small inserts with
// the shadow maps. It is, however, not responsible for the shadows to
// work in the first place.
//
// I do not really understand how it works.
//---------------------------------------------------------------------------
void OgreDemoQt::createShadowMapOverlays(void)
{
  Ogre::Hlms *hlmsUnlit = mRoot->getHlmsManager()->getHlms( Ogre::HLMS_UNLIT );

  Ogre::HlmsMacroblock macroblock;
  macroblock.mDepthCheck = false;
  Ogre::HlmsBlendblock blendblock;

  Ogre::CompositorShadowNode *shadowNode = mWorkspace->findShadowNode(
      "ShadowMapDebuggingShadowNode"
  );
  for( int i=0; i<5; ++i ) {
    const Ogre::String datablockName("depthShadow" + Ogre::StringConverter::toString(i));
    Ogre::HlmsUnlitDatablock *depthShadow =
      (Ogre::HlmsUnlitDatablock*)hlmsUnlit->createDatablock(
           datablockName, datablockName, macroblock, blendblock, Ogre::HlmsParamVec()
      );
    Ogre::TexturePtr tex = shadowNode->getLocalTextures()[i].textures[0];
    depthShadow->setTexture( 0, 0, tex, 0 );
  }

  Ogre::v1::OverlayManager &overlayManager = Ogre::v1::OverlayManager::getSingleton();
  // Create an overlay
  mOverlayPSSM       = overlayManager.create("PSSM Overlays");
  mOverlaySpotlights = overlayManager.create("Spotlight overlays");

  for( int i=0; i<3; ++i )
    {
      // Create a panel
      Ogre::v1::OverlayContainer* panel = static_cast<Ogre::v1::OverlayContainer*>(
          overlayManager.createOverlayElement(
              "Panel", "PanelName" + Ogre::StringConverter::toString(i)
          )
      );
      panel->setMetricsMode( Ogre::v1::GMM_RELATIVE_ASPECT_ADJUSTED );
      panel->setPosition( 100 + i * 1600, 10000 - 1600 );
      panel->setDimensions( 1500, 1500 );
      panel->setMaterialName( "depthShadow" + Ogre::StringConverter::toString(i));
      mOverlayPSSM->add2D( panel );
    }

  for( int i=3; i<5; ++i )
    {
      // Create a panel
      Ogre::v1::OverlayContainer* panel = static_cast<Ogre::v1::OverlayContainer*>(
          overlayManager.createOverlayElement(
              "Panel", "PanelName" + Ogre::StringConverter::toString(i)
          )
      );
      panel->setMetricsMode( Ogre::v1::GMM_RELATIVE_ASPECT_ADJUSTED );
      panel->setPosition( 100 + i * 1600, 10000 - 1600 );
      panel->setDimensions( 1500, 1500 );
      panel->setMaterialName( "depthShadow" + Ogre::StringConverter::toString(i));
      mOverlaySpotlights->add2D( panel );
    }

  mOverlayPSSM->show();
  mOverlaySpotlights->show();
}
