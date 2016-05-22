# Reference: http://qt-project.org/doc/qt-5.0/qtdoc/cmake-manual.html

cmake_minimum_required(VERSION 3.0)

set(APP_QT "qt_shadowmap")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(example VERSION 1.0)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Extra compiler flags.
add_definitions(-Wall -DNDEBUG)
#set(CMAKE_CXX_STANDARD 11)

# --------------------------------------------------

# Required packages.
include(QtCommon)
find_package(PkgConfig REQUIRED)
find_package(OGRE REQUIRED)
find_package(Qt5Widgets REQUIRED)
 
# --------------------------------------------------

link_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(
  ${OGRE_INCLUDE_DIRS}
  ${OGRE_Overlay_INCLUDE_DIRS}
  $ENV{CONDA_ENV_PATH}/include/OGRE/Hlms/
  $ENV{CONDA_ENV_PATH}/include/OGRE/Hlms/Pbs
  $ENV{CONDA_ENV_PATH}/include/OGRE/Hlms/Unlit
)

# --------------------------------------------------

set(SRC_QT
  src_qt/main.cpp
  src_qt/OgreDemoQt.cpp
)

# --------------------------------------------------

# Specify the executable.
add_executable(${APP_QT} ${SRC_QT})
qt5_use_modules(${APP_QT} Widgets)
target_link_libraries(
  ${APP_QT}
  ${OGRE_LIBRARIES}
  ${OGRE_Overlay_LIBRARIES}
  $ENV{CONDA_ENV_PATH}/lib/libOgreHlmsUnlit.so
  $ENV{CONDA_ENV_PATH}/lib/libOgreHlmsPbs.so
)
