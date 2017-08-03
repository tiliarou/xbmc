if(CORE_SYSTEM_NAME STREQUAL windows)
  set(SOURCES RPWinRenderer.cpp)
  set(HEADERS RPWinRenderer.h)
  core_add_library(rp-videorenderers)
endif()

