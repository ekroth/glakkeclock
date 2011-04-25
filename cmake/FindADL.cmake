# Find AMD Display Library (ADL)
# Once done this will define
# ADL_FOUND
# ADL_LIBRARY
# ADL_INCLUDE_DIR
# Glaucous 2011 <glakke1@gmail.com>

find_path(
ADL_INCLUDE_DIR

NAMES
adl_sdk.h
ADL/adl_sdk.h
adl/adl_sdk.h

PATHS
${CMAKE_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/src

DOC
"Directory where adl_sdk.h resides"
)

find_library(
ADL_LIBRARY

NAMES
atiadlxx
fglrx/${CMAKE_SHARED_LIBRARY_PREFIX}atiadlxx${CMAKE_SHARED_LIBRARY_SUFFIX}

PATHS

DOC
"ADL library location")

set(ADL_FOUND "FALSE")
if(ADL_LIBRARY AND ADL_INCLUDE_DIR)
	set(ADL_FOUND "TRUE")
endif()

if(${ADL_FOUND} MATCHES "FALSE" AND NOT ADL_FIND_QUIETLY)
	if(ADL_FIND_REQUIRED)
		message(FATAL_ERROR "ADL required, but wasn't found.")
	else()
		message(STATUS "ADL wasn't found.")
	endif()
endif()