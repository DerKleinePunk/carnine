# - Try to find Cairo
# Once done, this will define
#
#  GOBJECT_FOUND - system has Cairo
#  GOBJECT_INCLUDE_DIRS - the Cairo include directories
#  GOBJECT_LIBRARIES - link these to use Cairo
#
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PC_GOBJECT QUIET gobject-2.0)

FIND_PATH(GObject_INCLUDE_DIR
    NAMES gobject/gobject.h
    HINTS ${PC_GOBJECT_INCLUDEDIR}
          ${PC_GOBJECT_INCLUDE_DIRS}
          $ENV{GOBJECT_HOME}/include
          $ENV{GOBJECT_ROOT}/include
          /usr/local/include
          /usr/include
          /glib2/include
          /glib-2.0/include
    PATH_SUFFIXES glib2 glib-2.0 glib-2.0/include
)
SET(GObject_INCLUDE_DIRS ${GObject_INCLUDE_DIR})

FIND_LIBRARY(GObject_LIBRARIES
    NAMES gobject2 gobject-2.0
    HINTS ${PC_GOBJECT_LIBDIR}
          ${PC_GOBJECT_LIBRARY_DIRS}
          $ENV{GLIB2_HOME}/lib
          $ENV{GLIB2_ROOT}/lib
          /usr/local/lib
          /usr/lib
          /lib
          /glib-2.0/lib
    PATH_SUFFIXES glib2 glib-2.0
)

GET_FILENAME_COMPONENT(gobjectLibDir "${GOBJECT_LIBRARIES}" PATH)
FIND_PATH(GObject_CONFIG_INCLUDE_DIR
    NAMES glibconfig.h
    HINTS ${PC_GOBJECT_INCLUDEDIR}
          ${PC_GOBJECT_INCLUDE_DIRS}
          $ENV{GOBJECT_HOME}/include
          $ENV{GOBJECT_ROOT}/include
          /usr/local/include
          /usr/include
          /glib2/include
          /glib-2.0/include
          ${gOBJECTLibDir}
          ${CMAKE_SYSTEM_LIBRARY_PATH}
    PATH_SUFFIXES glib2 glib-2.0 glib-2.0/include
)
IF(GObject_CONFIG_INCLUDE_DIR)
    SET(GObject_INCLUDE_DIRS ${GObject_INCLUDE_DIRS} ${GObject_CONFIG_INCLUDE_DIR})
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GObject DEFAULT_MSG GObject_INCLUDE_DIRS GObject_LIBRARIES)
MARK_AS_ADVANCED(GObject_INCLUDE_DIR GObject_CONFIG_INCLUDE_DIR)
