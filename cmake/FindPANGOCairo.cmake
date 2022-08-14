# pangocairo
PKG_CHECK_MODULES(PC_PANGOCAIRO QUIET pangocairo)
FIND_PATH(PANGOCairo_INCLUDE_DIRS
    NAMES pango/pangocairo.h
    HINTS ${PC_PANGOCAIRO_INCLUDEDIR}
          ${PC_PANGOCAIRO_INCLUDE_DIRS}
          $ENV{PANGOCAIRO_HOME}/include
          $ENV{PANGOCAIRO_ROOT}/include
          /pangocairo/include
          ${PANGO_INCLUDE_HINTS}
    PATH_SUFFIXES pango pango-1.0 pangocairo libpangocairo-1.0 pangocairo1.0
)

FIND_LIBRARY(PANGOCairo_LIBRARIES
    NAMES pangocairo PANGOcairo PANGOcairo-1.0 pangocairo-1.0 libpangocairo-1.0
    HINTS ${PC_PANGOCAIRO_LIBDIR}
          ${PC_PANGOCAIRO_LIBRARY_DIRS}
          $ENV{PANGOCAIRO_HOME}/lib
          $ENV{PANGOCAIRO_ROOT}/lib
          ${PANGO_LIBRARY_HINTS}
    PATH_SUFFIXES pango pango-1.0 pangocairo libpangocairo-1.0 pangocairo1.0
)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(PANGOCairo DEFAULT_MSG PANGOCairo_INCLUDE_DIRS PANGOCairo_LIBRARIES)
MARK_AS_ADVANCED(PANGOCairo_INCLUDE_DIRS PANGOCairo_LIBRARIES)