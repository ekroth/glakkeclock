# Find AMD Display Library (ADL)
# Once done this will define
# ADL_FOUND
# ADL_LIBRARY
# ADL_INCLUDE_DIR
# Glaucous 2011 <glakke1 at gmail dot com>

FIND_PATH(
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

FIND_LIBRARY(
	ADL_LIBRARY

	NAMES
	atiadlxx
	atiadlxy

	PATHS
	/usr/lib/fglrx

	DOC
	"ADL library location")

IF (NOT ADL_LIBRARY)
	MESSAGE(STATUS "Library not found, finding file instead.")
	FIND_FILE(
	ADL_LIBRARY

	NAMES
	atiadlxx.dll
	atiadlxy.dll

	PATHS
	/usr/lib/fglrx

	DOC
	"ADL library location")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ADL DEFAULT_MSG ADL_LIBRARY ADL_INCLUDE_DIR)