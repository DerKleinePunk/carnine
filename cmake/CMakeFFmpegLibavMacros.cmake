#-------------------------------------------------------------------
# FindFFmpeg.cmake and FindLibAV.cmake are dependent on this file.
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file
#
#-------------------------------------------------------------------

### Macro: set_component_found
#
# Marks the given component as found if both *_LIBRARIES AND *_INCLUDE_DIRS is present.
#
macro(set_component_found _component )
  if (${_component}_LIBRARIES AND ${_component}_INCLUDE_DIRS)
    # message(STATUS "  - ${_component} found.")
    set(${_component}_FOUND TRUE)
  else ()
    # message(STATUS "  - ${_component} not found.")
  endif ()
endmacro()

#
### Macro: find_component
#
# Finds each component's library file and include directory, and sets
# *_LIBRARIES and *_INCLUDE_DIRS accordingly. Additionally detects each
# component's version and sets *_VERSION_STRING.
#
macro(find_component _component _library _header _version _version_required)

  SET(FFMPEG_SEARCH_PATHS
    ${FFMPEG_SRC}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/lib/arm-linux-gnueabihf
    /usr/include/arm-linux-gnueabihf
    /usr/local
    /usr
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
    /F/Mine/OpenSource/ffmpeg/ffmpeg-3.4.1-win32-dev
    F:/Mine/OpenSource/ffmpeg/ffmpeg-3.4.1-win32-dev
  )

  MESSAGE(STATUS "search for ${_component} in ${FFMPEG_SEARCH_PATHS}")

  find_path(${_component}_INCLUDE_DIRS ${_header}
    PATH_SUFFIXES
      ffmpeg
      libav
      include
    PATHS ${FFMPEG_SEARCH_PATHS}
  )

  find_library(${_component}_LIBRARIES
      NAMES ${_library}
      PATH_SUFFIXES
        lib
      PATHS ${FFMPEG_SEARCH_PATHS}
  )

  if (${_component}_INCLUDE_DIRS AND EXISTS "${${_component}_INCLUDE_DIRS}/${_version}")
    file(STRINGS "${${_component}_INCLUDE_DIRS}/${_version}" ${_component}_VERSION_MAJOR_LINE REGEX "^#define[ \t]+LIB${_component}_VERSION_MAJOR[ \t]+[0-9]+$")
    file(STRINGS "${${_component}_INCLUDE_DIRS}/${_version}" ${_component}_VERSION_MINOR_LINE REGEX "^#define[ \t]+LIB${_component}_VERSION_MINOR[ \t]+[0-9]+$")
    file(STRINGS "${${_component}_INCLUDE_DIRS}/${_version}" ${_component}_VERSION_PATCH_LINE REGEX "^#define[ \t]+LIB${_component}_VERSION_MICRO[ \t]+[0-9]+$")
    string(REGEX REPLACE "^#define[ \t]+LIB${_component}_VERSION_MAJOR[ \t]+([0-9]+)$" "\\1" ${_component}_VERSION_MAJOR "${${_component}_VERSION_MAJOR_LINE}")
    string(REGEX REPLACE "^#define[ \t]+LIB${_component}_VERSION_MINOR[ \t]+([0-9]+)$" "\\1" ${_component}_VERSION_MINOR "${${_component}_VERSION_MINOR_LINE}")
    string(REGEX REPLACE "^#define[ \t]+LIB${_component}_VERSION_MICRO[ \t]+([0-9]+)$" "\\1" ${_component}_VERSION_PATCH "${${_component}_VERSION_PATCH_LINE}")
    set(${_component}_VERSION_STRING ${${_component}_VERSION_MAJOR}.${${_component}_VERSION_MINOR}.${${_component}_VERSION_PATCH})
    unset(${_component}_VERSION_MAJOR_LINE)
    unset(${_component}_VERSION_MINOR_LINE)
    unset(${_component}_VERSION_PATCH_LINE)
    unset(${_component}_VERSION_MAJOR)
    unset(${_component}_VERSION_MINOR)
    unset(${_component}_VERSION_PATCH)
    
    set(VERSION_IS ${${_component}_VERSION_STRING})
    message(STATUS "${_component} found Version ${VERSION_IS}")
    if (${VERSION_IS} VERSION_LESS ${_version_required})
       set(VERSION_NEED ${_version_required})
       message(STATUS "${_component} too old found ${VERSION_IS} need ${VERSION_NEED}")
    else()
       set(CMAKE_FIND_PACKAGE_NAME ${_component})
       find_package_handle_standard_args(${_component}
                                        REQUIRED_VARS ${_component}_LIBRARIES ${_component}_INCLUDE_DIRS
                                        VERSION_VAR ${_component}_VERSION_STRING)

       set_component_found(${_component})
    endif () 
  endif ()
endmacro()
