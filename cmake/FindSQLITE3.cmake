# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
#	Note:
#		Searching headers and libraries is very simple and is NOT as powerful as scripts
#		distributed with CMake, because LuaDist defines directories to search for.
#		Everyone is encouraged to contact the author with improvements. Maybe this file
#		becomes part of CMake distribution sometimes.

# - Find sqlite3
# Find the native SQLITE3 headers and libraries.
#
# SQLITE3_INCLUDE_DIRS	- where to find sqlite3.h, etc.
# SQLITE3_LIBRARIES	- List of libraries when using sqlite.
# SQLITE3_FOUND	- True if sqlite found.

SET(SQLITE3_SEARCH_PATHS
    ${SQLITE3_SRC}
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
    /usr/lib/arm-linux-gnueabihf/
	$ENV{VCPKG_ROOT}\\installed\\x86-windows
)
    
# Look for the header file.
FIND_PATH(SQLITE3_INCLUDE_DIR NAMES sqlite3.h
    HINTS
	$ENV{SQLITE3DIR}
	PATH_SUFFIXES include/SQLITE3 include
	PATHS ${SQLITE3_SEARCH_PATHS})

# Look for the library.
FIND_LIBRARY(SQLITE3_LIBRARY NAMES sqlite3
    HINTS
	$ENV{SDL2DIR}
	PATH_SUFFIXES lib
	PATHS ${SQLITE3_SEARCH_PATHS}
)

# Extract version information from the header file
if(SQLITE3_INCLUDE_DIR)
    file(STRINGS ${SQLITE3_INCLUDE_DIR}/sqlite3.h _ver_line
         REGEX "^#define SQLITE_VERSION  *\"[0-9]+\\.[0-9]+\\.[0-9]+\""
         LIMIT_COUNT 1)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+"
		 SQLITE3_VERSION "${_ver_line}")
    unset(_ver_line)
endif()

# Handle the QUIETLY and REQUIRED arguments and set SQLITE3_FOUND to TRUE if all listed variables are TRUE.
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SQLITE3 DEFAULT_MSG SQLITE3_LIBRARY SQLITE3_INCLUDE_DIR SQLITE3_VERSION)

# Copy the results to the output variables.
IF(SQLITE3_FOUND)
	SET(SQLITE3_LIBRARIES ${SQLITE3_LIBRARY})
	SET(SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
ELSE(SQLITE3_FOUND)
	SET(SQLITE3_LIBRARIES)
	SET(SQLITE3_INCLUDE_DIRS)
ENDIF(SQLITE3_FOUND)

MARK_AS_ADVANCED(SQLITE3_INCLUDE_DIRS SQLITE3_LIBRARIES)
