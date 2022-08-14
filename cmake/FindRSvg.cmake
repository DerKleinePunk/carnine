# - try to find RSvg

if(RSVG_INCLUDE_DIRS AND RSVG_LIBRARIES)

  # in cache already
  set(RSvg_FOUND TRUE)

else(RSVG_INCLUDE_DIRS AND RSVG_LIBRARIES)

if(NOT WIN32)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(_pc_rsvg librsvg-2.0)
  # pkg_search_module(GDKPIXBUF gdk-pixbuf-2.0)
  if(_pc_rsvg_FOUND)
    set(RSvg_FOUND TRUE)
  endif(_pc_rsvg_FOUND)
else(NOT WIN32)
  # assume so, for now
  set(RSvg_FOUND TRUE)
endif(NOT WIN32)

if(RSvg_FOUND)
  # set it back as false
  set(RSvg_FOUND FALSE)

  find_library(RSVG_LIBRARY rsvg-2
               HINTS ${_pc_rsvg_LIBRARY_DIRS}
  )
  set(RSVG_LIBRARIES "${RSVG_LIBRARY}")

  find_path(RSVG_INCLUDE_DIR rsvg.h
            HINTS ${_pc_rsvg_INCLUDE_DIRS}
            PATH_SUFFIXES librsvg
  )
  
  find_path(PIXBUF_INCLUDE_DIR gdk-pixbuf/gdk-pixbuf.h
          HINTS ${_pc_rsvg_INCLUDE_DIRS}
  )
 
  set(RSVG_INCLUDE_DIRS ${RSVG_INCLUDE_DIR} ${PIXBUF_INCLUDE_DIR})

  MESSAGE(STATUS "RSvg ${RSVG_INCLUDE_DIRS}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(RSvg DEFAULT_MSG RSVG_LIBRARIES RSVG_INCLUDE_DIRS)
endif(RSvg_FOUND)

endif(RSVG_INCLUDE_DIRS AND RSVG_LIBRARIES)

mark_as_advanced(
        RSVG_CFLAGS
        RSVG_INCLUDE_DIRS
        RSVG_LIBRARIES
)
