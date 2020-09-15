# Install script for directory: F:/Yan/8/assignment8/CGL/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/RopeSim")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "F:/Yan/8/assignment8/build/CGL/src/CGL.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/CGL" TYPE FILE FILES
    "F:/Yan/8/assignment8/CGL/src/CGL.h"
    "F:/Yan/8/assignment8/CGL/src/vector2D.h"
    "F:/Yan/8/assignment8/CGL/src/vector3D.h"
    "F:/Yan/8/assignment8/CGL/src/vector4D.h"
    "F:/Yan/8/assignment8/CGL/src/matrix3x3.h"
    "F:/Yan/8/assignment8/CGL/src/matrix4x4.h"
    "F:/Yan/8/assignment8/CGL/src/quaternion.h"
    "F:/Yan/8/assignment8/CGL/src/complex.h"
    "F:/Yan/8/assignment8/CGL/src/color.h"
    "F:/Yan/8/assignment8/CGL/src/osdtext.h"
    "F:/Yan/8/assignment8/CGL/src/viewer.h"
    "F:/Yan/8/assignment8/CGL/src/base64.h"
    "F:/Yan/8/assignment8/CGL/src/tinyxml2.h"
    "F:/Yan/8/assignment8/CGL/src/renderer.h"
    )
endif()

