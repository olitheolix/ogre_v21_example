# Reference: http://qt-project.org/doc/qt-5.0/qtdoc/cmake-manual.html

cmake_minimum_required(VERSION 3.0)

set(APP_SDL "sdl_shadowmap")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(example VERSION 1.0)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Extra compiler flags.
add_definitions(-Wall -DNDEBUG)
#set(CMAKE_CXX_STANDARD 11)

# --------------------------------------------------

# Required packages.
find_package(PkgConfig REQUIRED)
find_package(OGRE REQUIRED)
find_package(SDL2 REQUIRED)
 
# --------------------------------------------------

link_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(
  ${SDL2_INCLUDE_DIR}
  ${OGRE_INCLUDE_DIRS}
  ${OGRE_Overlay_INCLUDE_DIRS}
  $ENV{CONDA_ENV_PATH}/include/OGRE/Hlms/
  $ENV{CONDA_ENV_PATH}/include/OGRE/Hlms/Pbs
  $ENV{CONDA_ENV_PATH}/include/OGRE/Hlms/Unlit
)

# --------------------------------------------------

set(SRC_SDL
  src_sdl/OgreDemoSDL.cpp
)

# --------------------------------------------------

#SET (MYLIB -Wl,-rpath,/usr/lib/x86_64-linux-gnu/liblzma.so)

# Specify the executable and its dependencies.
add_executable(${APP_SDL} ${SRC_SDL})
target_link_libraries(
  ${APP_SDL}
  ${SDL2_LIBRARY}
  ${OGRE_LIBRARIES}
  ${OGRE_Overlay_LIBRARIES}
  $ENV{CONDA_ENV_PATH}/lib/libOgreHlmsUnlit.so
  $ENV{CONDA_ENV_PATH}/lib/libOgreHlmsPbs.so
#  -Wl,-rpath,/lib/x86_64-linux-gnu/liblzma.so.5
#  -Wl,--allow-shlib-undefined
#  liblzma.so
#  ${MYLIB}
)

