#.rst:
# FindOpenGlSwitch
# ----------
# Finds the OpenGlSwitch library
#
# This will define the following variables::
#
# OPENGLSWITCH_FOUND - system has OpenGlSwitch
# OPENGLSWITCH_INCLUDE_DIRS - the OpenGlSwitch include directory
# OPENGLSWITCH_LIBRARIES - the OpenGlSwitch libraries
# OPENGLSWITCH_DEFINITIONS - the OpenGlSwitch definitions

if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_OPENGLSWITCH gl glu QUIET)
endif()

find_path(OPENGLSWITCH_INCLUDE_DIR GL/gl.h
                            PATHS ${PC_OPENGLSWITCH_gl_INCLUDEDIR})
find_library(OPENGLSWITCH_gl_LIBRARY NAMES libdrm_nouveau.a
                                PATHS ${PC_OPENGLSWITCH_gl_LIBDIR})

message("OPENGLSWITCH_INCLUDE_DIR is ${OPENGLSWITCH_INCLUDE_DIR}")
message("OPENGLSWITCH_gl_LIBRARY is ${OPENGLSWITCH_gl_LIBRARY}")
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenGlSwitch
                                  REQUIRED_VARS OPENGLSWITCH_gl_LIBRARY OPENGLSWITCH_INCLUDE_DIR)

if(OPENGLSWITCH_FOUND)
  set(OPENGLSWITCH_INCLUDE_DIRS ${OPENGLSWITCH_INCLUDE_DIR})
  set(OPENGLSWITCH_LIBRARIES ${OPENGLSWITCH_gl_LIBRARY})
  set(OPENGLSWITCH_DEFINITIONS -DHAS_GL=1)
endif()

mark_as_advanced(OPENGLSWITCH_INCLUDE_DIR OPENGLSWITCH_gl_LIBRARY)
