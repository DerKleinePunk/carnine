project("CarNiNe" VERSION "${RELEASE_VERSION}" LANGUAGES CXX)

FIND_PACKAGE(OSMScout REQUIRED)
FIND_PACKAGE(Cairo REQUIRED)
FIND_PACKAGE(PANGO REQUIRED)
FIND_PACKAGE(RSvg REQUIRED)
FIND_PACKAGE(FFmpeg COMPONENTS AVFORMAT AVCODEC AVUTIL SWSCALE SWRESAMPLE REQUIRED)
FIND_PACKAGE(SQLITE3 REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBOSMSCOUT")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLEAUDIOMANAGER")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLEMUSIKMANAGER")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLECAIRO")

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SDL2_NET_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${OSMSCOUT_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CAIRO_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PANGO_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${RSVG_INCLUDE_DIRS})
INCLUDE_DIRECTORIES("${MAINFOLDER}/src/frontend")

MESSAGE(STATUS "Try Build ${PROJECT_NAME} in Version ${RELEASE_VERSION}")

## Use all the *.cpp files we found under this folder for the project Find better way for coomen cpp files
FILE(GLOB SRCS "*.cpp" "*/*.cpp")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    SET (project_BIN ${PROJECT_NAME})
else()
    SET (project_BIN ${PROJECT_NAME}.bin)
endif()

## Define the executable
add_executable(${project_BIN} ${SRCS} ${GUI_SRCS} ${MAIN_common_SRCS} ${easylogging_SRCS} ${common_utils_SRCS} ${common_utils_SRCS_LOGGER} ${common_utils_communication_SRCS} ${MAP_SRCS} ${common_utils_SRCS_OSMSCOUT} ${common_database_SRCS} ${common_utils_SRCS_SDL2} ${common_SDL2_gfx_SRCS} ${SOUND_MUSIK_SRCS})

TARGET_LINK_LIBRARIES(${project_BIN} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} ${OSMScout_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} ${CAIRO_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} ${PANGO_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} ${RSVG_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} ${FFMPEG_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} ${SQLITE3_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} ${SDL2_MIXER_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} ${SDL2_NET_LIBRARIES})

message(STATUS "Generating Copy Rule for Resources")

set(MAINFONTSOURCE "${MAINFOLDER}/src/frontend/logger.conf")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                       "${MAINFONTSOURCE}"      # <--this is in-file
                       $<TARGET_FILE_DIR:${project_BIN}>)
set(MAINFONTSOURCE "${MAINFOLDER}/resources/font/Inconsolata-Bold.ttf")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                       "${MAINFONTSOURCE}"      # <--this is in-file
                       $<TARGET_FILE_DIR:${project_BIN}>)
set(MAINFONTSOURCE "${MAINFOLDER}/resources/font/Inconsolata-Regular.ttf")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                       "${MAINFONTSOURCE}"      # <--this is in-file
                       $<TARGET_FILE_DIR:${project_BIN}>)