
SET(OSMScout_SEARCH_PATHS
~/Library/Frameworks
/Library/Frameworks
/usr/local
/usr
/sw # Fink
/opt/local # DarwinPorts
/opt/csw # Blastwave
/opt
${OSMSCOUT_PATH}
$ENV{OSMSCOUT_PATH}
F:/Mine/OpenSource/libosmscout-code/build/MinGW/Debug
F:/Mine/OpenSource/libosmscout-code/build/MinGW/Release
)

MESSAGE(STATUS "OSMScout ${OSMScout_SEARCH_PATHS}")

if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
set(PATH_SUFFIXES lib64 lib/x64 lib)
else() 
set(PATH_SUFFIXES lib/x86 lib)
endif() 

FIND_PATH(OSMScout_INCLUDE_DIR osmscout/Location.h
HINTS
PATH_SUFFIXES include
PATHS ${OSMScout_SEARCH_PATHS}
)

FIND_PATH(OSMScout_INCLUDE_CAIRO_DIR osmscoutmapcairo/MapPainterCairo.h
    HINTS
    PATH_SUFFIXES include
    PATHS ${OSMScout_SEARCH_PATHS}
    )

FIND_PATH(OSMScout_INCLUDE_MAP_DIR osmscoutmap/MapService.h
    HINTS
    PATH_SUFFIXES include
    PATHS ${OSMScout_SEARCH_PATHS}
    )


FIND_LIBRARY(OSMScout_LIBRARY_DEBUG
NAMES osmscoutd
HINTS
    ENV OSMSCOUT_PATH
PATH_SUFFIXES lib lib64
PATHS ${OSMScout_SEARCH_PATHS}
)

FIND_LIBRARY(OSMScout_LIBRARY_RELEASE
NAMES osmscout
HINTS
    ENV OSMSCOUT_PATH
PATH_SUFFIXES lib lib64
PATHS ${OSMScout_SEARCH_PATHS}
)

FIND_LIBRARY(OSMScout_LIBRARY_MAP_DEBUG
NAMES osmscout_mapd
HINTS
    ENV OSMSCOUT_PATH
PATH_SUFFIXES lib lib64
PATHS ${OSMScout_SEARCH_PATHS}
)

FIND_LIBRARY(OSMScout_LIBRARY_MAP_RELEASE
NAMES osmscout_map
HINTS
    ENV OSMSCOUT_PATH
PATH_SUFFIXES lib lib64
PATHS ${OSMScout_SEARCH_PATHS}
)

FIND_LIBRARY(OSMScout_LIBRARY_MAP_CAIRO_DEBUG
NAMES osmscout_map_cairod
HINTS
    ENV OSMSCOUT_PATH
PATH_SUFFIXES lib lib64
PATHS ${OSMScout_SEARCH_PATHS}
)

FIND_LIBRARY(OSMScout_LIBRARY_MAP_CAIRO_RELEASE
NAMES osmscout_map_cairo
HINTS
    ENV OSMSCOUT_PATH
PATH_SUFFIXES lib lib64
PATHS ${OSMScout_SEARCH_PATHS}
)

INCLUDE(FindPackageHandleStandardArgs)

SET(OSMScout_LIBRARIES_DEBUG ${OSMScout_LIBRARY_DEBUG} ${OSMScout_LIBRARY_MAP_DEBUG} ${OSMScout_LIBRARY_MAP_CAIRO_DEBUG})
if(NOT OSMScout_LIBRARY_DEBUG AND OSMScout_LIBRARY_RELEASE)
set(OSMScout_LIBRARY_DEBUG ${OSMScout_LIBRARY_RELEASE})
endif()
if(NOT OSMScout_LIBRARY_MAP_DEBUG AND OSMScout_LIBRARY_MAP_RELEASE)
set(OSMScout_LIBRARY_MAP_DEBUG ${OSMScout_LIBRARY_MAP_RELEASE})
endif()
if(NOT OSMScout_LIBRARY_MAP_CAIRO_DEBUG AND OSMScout_LIBRARY_MAP_CAIRO_RELEASE)
set(OSMScout_LIBRARY_MAP_CAIRO_DEBUG ${OSMScout_LIBRARY_MAP_CAIRO_RELEASE})
endif()
MESSAGE(STATUS "OSMScout debug ${OSMScout_LIBRARIES_DEBUG}")
SET(OSMScout_LIBRARIES_RELEASE ${OSMScout_LIBRARY_RELEASE} ${OSMScout_LIBRARY_MAP_RELEASE} ${OSMScout_LIBRARY_MAP_CAIRO_RELEASE})
MESSAGE(STATUS "OSMScout release ${OSMScout_LIBRARIES_RELEASE}")
set(OSMScout_INCLUDE_DIRS ${OSMScout_INCLUDE_DIR} ${OSMScout_INCLUDE_CAIRO_DIR} ${OSMScout_INCLUDE_MAP_DIR})
MESSAGE(STATUS "OSMScout ${OSMScout_INCLUDE_DIRS}")

MESSAGE(STATUS "OSMScout Buildtype ${CMAKE_BUILD_TYPE}")

set(OSMScout_LIBRARIES
    debug ${OSMScout_LIBRARY_DEBUG} 
    debug ${OSMScout_LIBRARY_MAP_DEBUG}
    debug ${OSMScout_LIBRARY_MAP_CAIRO_DEBUG}
    optimized ${OSMScout_LIBRARY_RELEASE}
    optimized ${OSMScout_LIBRARY_MAP_RELEASE}
    optimized ${OSMScout_LIBRARY_MAP_CAIRO_RELEASE}
)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OSMScout REQUIRED_VARS OSMScout_LIBRARIES OSMScout_INCLUDE_DIRS)