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

#include "OgreDemoSDL.hpp"

#include "OgreFrameStats.h"

#include "OgreItem.h"
#include "OgreCamera.h"
#include "OgreRenderWindow.h"

#include "OgreArchiveManager.h"

#include "OgreHlmsPbs.h"
#include "OgreHlmsUnlit.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsUnlitDatablock.h"

#include "OgreMesh2.h"
#include "OgreMeshManager.h"
#include "OgreMeshManager2.h"

#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"

#include "Compositor/OgreCompositorManager2.h"
#include "Compositor/OgreCompositorWorkspace.h"
#include "Compositor/OgreCompositorShadowNode.h"

#include "OgreWindowEventUtilities.h"

#include <SDL_syswm.h>


//-----------------------------------------------------------------------------------
// Constructor.
//-----------------------------------------------------------------------------------
OgreDemoSDL::OgreDemoSDL():
  mSdlWindow(NULL),
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
  mQuit(false)
{}

//-----------------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------------
OgreDemoSDL::~OgreDemoSDL()
{
  assert( !mRoot && "shutdown method was not called!!!" );
}

//-----------------------------------------------------------------------------------
// Setup the render window and scene manager. Then load the resources.
//-----------------------------------------------------------------------------------
void OgreDemoSDL::initialiseSDLWindow()
{
  // Specify window title.
  Ogre::String windowTitle("Ogre 2.1: Shadow Map Demo (SDL)");

  Ogre::ConfigOptionMap& cfgOpts = mRoot->getRenderSystem()->getConfigOptions();

  // Default values (may be overwritten below based on values in config file).
  int width  = 640;
  int height = 400;

  Ogre::ConfigOptionMap::iterator opt = cfgOpts.find( "Video Mode" );
  if( opt != cfgOpts.end() ) {
    // Ignore leading space
    const Ogre::String::size_type start =
      opt->second.currentValue.find_first_of("012356789");

    //Get the width and height
    Ogre::String::size_type widthEnd = opt->second.currentValue.find(' ', start);

    // we know that the height starts 3 characters after the width and goes
    // until the next space
    Ogre::String::size_type heightEnd = opt->second.currentValue.find(' ', widthEnd+3);

    // Now we can parse out the values
    width = Ogre::StringConverter::parseInt(opt->second.currentValue.substr(0, widthEnd));
    height= Ogre::StringConverter::parseInt(
                opt->second.currentValue.substr(widthEnd+3, heightEnd)
    );
  }

  int screen = 0;
  int posX = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);
  int posY = SDL_WINDOWPOS_CENTERED_DISPLAY(screen);

  bool fullscreen = Ogre::StringConverter::parseBool(cfgOpts["Full Screen"].currentValue);
  if(fullscreen) {
    posX = SDL_WINDOWPOS_UNDEFINED_DISPLAY(screen);
    posY = SDL_WINDOWPOS_UNDEFINED_DISPLAY(screen);
  }

  mSdlWindow = SDL_CreateWindow(
      windowTitle.c_str(),    // window title
      posX,                   // initial x position
      posY,                   // initial y position
      width,                  // width, in pixels
      height,                 // height, in pixels
      SDL_WINDOW_SHOWN
      | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_RESIZABLE
  );

  // Get the native window handle.
  SDL_SysWMinfo wmInfo;
  SDL_VERSION( &wmInfo.version );

  if( SDL_GetWindowWMInfo( mSdlWindow, &wmInfo ) == SDL_FALSE ) {
    OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR,
                 "Couldn't get WM Info! (SDL2)",
                 "OgreDemoSDL::initialize" );
  }

  Ogre::String winHandle;
  Ogre::NameValuePairList params;

  switch( wmInfo.subsystem ) {
  case SDL_SYSWM_X11:
    winHandle = Ogre::StringConverter::toString( (uintptr_t)wmInfo.info.x11.window );
    break;
  default:
    OGRE_EXCEPT( Ogre::Exception::ERR_NOT_IMPLEMENTED,
                 "Unexpected WM! (SDL2)",
                 "OgreDemoSDL::initialize" );
    break;
  }

  params.insert( std::make_pair("title", windowTitle) );
  params.insert( std::make_pair("gamma", "true") );
  params.insert( std::make_pair("FSAA", cfgOpts["FSAA"].currentValue) );
  params.insert( std::make_pair("vsync", cfgOpts["VSync"].currentValue) );

  params.insert( std::make_pair("parentWindowHandle",  winHandle) );
  mRenderWindow = Ogre::Root::getSingleton().createRenderWindow(
      windowTitle, width, height, fullscreen, &params);
}


int OgreDemoSDL::initialise()
{
  // Ogre's plugin file (contains path to render libraries).
  char fname_config[] = "plugins.cfg";

  // Verify that the file exists. Abort with an error if does not.
  if (!std::ifstream(fname_config)) {
    printf("Error: could not find <plugins.cfg> in current directory.\n"
           "Run <create_config.sh> in root directory to create one.\n"
           "Then start this demo (from root directory) with "
           "$ ./sdl/build/sdl_shadowmap\n\n"
           "Demo aborted\n");
    return 1;
  }

  // No idea.
  if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ) {
    OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR, "Cannot initialise SDL2!",
                 "OgreDemoSDL::initialize" );
  }

  // Create a new Root instance. It needs to know the location of
  // `plugins.cfg` (contains the renderer, eg `RenderSystem_GL3Plus`),
  // the configuration file, and the log file. Only `plugins.cfg` must
  // exist, the other files will be created if necessary.
  mRoot = OGRE_NEW Ogre::Root( fname_config, "ogre.cfg", "Ogre.log" );

  // Show the configuration dialog to the user. Abort immediately if
  // the user cancelled it.
  if( !mRoot->showConfigDialog() ) {
    mQuit = true;
    return 0;
  }

  // No idea.
  mRoot->getRenderSystem()->setConfigOption( "sRGB Gamma Conversion", "Yes" );
  mRoot->initialise(false);

  // SDL specific initialisations.
  initialiseSDLWindow();

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
// Graceful shutdown of Ogre. Should probably be called in destructor.
//---------------------------------------------------------------------------
void OgreDemoSDL::shutdown(void)
{
  if( mSceneManager )
    mSceneManager->removeRenderQueueListener( mOverlaySystem );

  OGRE_DELETE mOverlaySystem;
  mOverlaySystem = 0;

  OGRE_DELETE mRoot;
  mRoot = 0;

  if( mSdlWindow ) {
    // Restore desktop resolution on exit
    SDL_SetWindowFullscreen( mSdlWindow, 0 );
    SDL_DestroyWindow( mSdlWindow );
    mSdlWindow = 0;
  }

  SDL_Quit();
}

//---------------------------------------------------------------------------
// Add a static plane.
//
// It will serve as the "ground" to better show of the shadows.
// See the Doxygen documentation for what the parameters mean
// exactly. Also note that this uses the Plane API from Ogre 1.x
// since it appears to not have been ported to Ogre 2.x yet (?).
//---------------------------------------------------------------------------
void OgreDemoSDL::addPlane(void)
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
void OgreDemoSDL::addCubes(void)
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
void OgreDemoSDL::addLights(void)
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
void OgreDemoSDL::createScene(void)
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

//---------------------------------------------------------------------------
// Rotate the cubes and update the FPS counter to the latest value.
//---------------------------------------------------------------------------
void OgreDemoSDL::updateScene( )
{
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

//---------------------------------------------------------------------------
// Set the `mQuit` flag if the window received a request to close
// itself (typically when the user presses <alt>-<F4>).
// 
// The render loop will check the mQuit flag and gracefully shut
// down Ogre if it is set.
//---------------------------------------------------------------------------
void OgreDemoSDL::checkCloseWindow( void )
{
  SDL_Event evt;
  while( SDL_PollEvent( &evt ) ) {
    switch( evt.type ) {
    case SDL_QUIT:
      mQuit = true;
      break;
    default:
      break;
    }
  }
}

//---------------------------------------------------------------------------
// Specify resource locations where Ogre can find meshes and textures.
//
// Note: these folders are usually specified in a `resources.cfg`
// file but parsing that file in C++ is... painful, even with the
// helper methods provided by Ogre. This demo uses hard coded paths
// for simplicity.
//---------------------------------------------------------------------------
void OgreDemoSDL::setResourceLocations(void)
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
void OgreDemoSDL::registerHlms(void)
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
void OgreDemoSDL::createSceneManager(void)
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

  // No idea, but if you comment the following line the overlays
  // will not appear.
  mSceneManager->addRenderQueueListener( mOverlaySystem );

  // Set sane defaults for proper shadow mapping.
  mSceneManager->setShadowDirectionalLightExtrusionDistance( 500.0f );
  mSceneManager->setShadowFarDistance( 500.0f );
}

//---------------------------------------------------------------------------
// Create a camera - nothing special.
//---------------------------------------------------------------------------
void OgreDemoSDL::createCamera(void)
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

//---------------------------------------------------------------------------
// Render the text with shadows.
//
// The shadow effect is simple: first render the entire text in black, then
// render the same text again in white but with a tiny position offset.
//---------------------------------------------------------------------------
void OgreDemoSDL::createTextOverlay(void)
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
  // "0DebugTextShadow") or otherwise it seems to get rendered first.
  // No idea why.
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
void OgreDemoSDL::createShadowMapOverlays(void)
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

//---------------------------------------------------------------------------
// This is the main method of this object. It will setup Ogre,
// create the window, and populate and update the scene. It will not
// return until the user closes the window.
//---------------------------------------------------------------------------
int OgreDemoSDL::start(void)
{
  // Setup the window, resources, scene manager, and camera.
  if (initialise() != 0) return 1;

  // Abort if the user cancelled the little configuration pop-up window.
  if( mQuit ) {
    shutdown();
    return 0;
  }

  // Populate the scene with meshes and lights.
  createScene();

  // Render the scene until the user closes the window.
  while( !mQuit ) {
    // Update the cube orientations and text (eg current FPS value).
    updateScene();

    // Trigger Ogre's message pump (necessary to render a frame).
    Ogre::WindowEventUtilities::messagePump();

    // Render one frame if the window is visible (otherwise do nothing).
    if( mRenderWindow->isVisible() ) mQuit |= !mRoot->renderOneFrame();
    else Ogre::Threads::Sleep( 500 );

    // Check in with SDL if the window received a <close> event (ie ALT-F4).
    checkCloseWindow();
  }

  // Clean up.
  shutdown();
  return 0;
}


int main()
{
  OgreDemoSDL graphicsSystem;
  return graphicsSystem.start();
}
