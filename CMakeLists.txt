cmake_minimum_required(VERSION 2.6)
project(glakkeclock)

set(KKE_VERSION 0.11.00 CACHE STRING "Program version")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

if(UNIX)
  add_definitions("-DLINUX")
elseif(WINDOWS)
  add_definitions("-DWINDOWS")
else()
  message(FATAL_ERROR "Operating System ${CMAKE_SYSTEM_NAME} not supported.")
endif()

if(NOT EXISTS ${CMAKE_INSTALL_PREFIX})
  set(CMAKE_INSTALL_PREFIX  "/usr")
endif()

# Kdevelop workaround
if(NOT DEFINED ${KKE_ARCH})
	set(KKE_ARCH "x86_64" CACHE STRING "Architecture")
endif()

if(${KKE_ARCH} MATCHES "x86_64")
  add_definitions("-m64")
elseif(${KKE_ARCH} MATCHES "x86")
  add_definitions("-m32")
else()
  message(FATAL_ERROR "Wrong architecture, ${KKE_ARCH}! (x86_64 or x86)")
endif()

find_package(ADL REQUIRED)

add_definitions("-Wall -DADLLOC='\"${ADL_LIBRARY}\"' -DVERSION='\"${KKE_VERSION}\"'")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-DRELEASE -O2")

set(KKE_BIN glakkeclock CACHE STRING "Filename")

add_executable(
${KKE_BIN}

src/ArgParser/ArgParser.cpp
src/ArgParser/Argument.cpp

src/main.cpp
src/kkeADL.cpp
src/GlakkeClock.cpp
src/Device.cpp
)

include_directories(
"${CMAKE_SOURCE_DIR}/src"
${ADL_INCLUDE_DIR})

target_link_libraries(
${KKE_BIN} 

${ADL_LIBRARY})

INSTALL(
TARGETS ${KKE_BIN}
RUNTIME DESTINATION bin)