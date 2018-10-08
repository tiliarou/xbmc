set(ARCH_DEFINES -DSWITCH -DTARGET_SWITCH)
#set(SYSTEM_DEFINES -D__STDC_CONSTANT_MACROS -D_FILE_DEFINED
#                   -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64)
set(PLATFORM_DIR platform/switch)
#set(CMAKE_SYSTEM_NAME Switch)
if(WITH_ARCH)
  set(ARCH ${WITH_ARCH})
else()
    set(ARCH aarch64)
    set(NEON True)
    set(NEON_FLAGS "-fPIC -mcpu=cortex-a57")
endif()

# -------- Paths (mainly for find_package) ---------
set(PLATFORM_DIR platform/switch)
set(APP_RENDER_SYSTEM gl)

set(CORE_MAIN_SOURCE ${CMAKE_SOURCE_DIR}/xbmc/platform/switch/main.cpp)


if((CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
    AND CMAKE_COMPILER_IS_GNUCXX)
  # Make sure we strip binaries in Release build
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

  # LTO Support, requires cmake >= 3.9
  if(CMAKE_VERSION VERSION_EQUAL 3.9.0 OR CMAKE_VERSION VERSION_GREATER 3.9.0)
    option(USE_LTO "Enable link time optimization. Specify an int for number of parallel jobs" OFF)
    if(USE_LTO)
      include(CheckIPOSupported)
      check_ipo_supported(RESULT HAVE_LTO OUTPUT _output)
      if(HAVE_LTO)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        # override flags to enable parallel processing
        set(NJOBS 2)
        if(USE_LTO MATCHES "^[0-9]+$")
          set(NJOBS ${USE_LTO})
        endif()
        set(CMAKE_CXX_COMPILE_OPTIONS_IPO -flto=${NJOBS} -fno-fat-lto-objects)
        set(CMAKE_C_COMPILE_OPTIONS_IPO -flto=${NJOBS} -fno-fat-lto-objects)
      else()
        message(WARNING "LTO optimization not supported: ${_output}")
        unset(_output)
      endif()
    endif()
  endif()
endif()

if(KODI_DEPENDSBUILD)
  # Binaries should be directly runnable from host, so include rpath to depends
  set(CMAKE_INSTALL_RPATH "${DEPENDS_PATH}/lib")
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

find_package(CXX11 REQUIRED)
include(LDGOLD)

if(ENABLE_GBM)
  set(ENABLE_VDPAU OFF CACHE BOOL "Disabling VDPAU" FORCE)
endif()

if(ENABLE_VDPAU)
  set(ENABLE_GLX OFF CACHE BOOL "Enabling GLX" FORCE)
endif()

set(SWITCH 1)
