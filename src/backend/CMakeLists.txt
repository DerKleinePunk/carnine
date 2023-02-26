project("CarNiNeBackend" VERSION "${RELEASE_VERSION}" LANGUAGES CXX)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${SDL2_NET_INCLUDE_DIRS})

MESSAGE(STATUS "Try Build ${PROJECT_NAME} in Version ${RELEASE_VERSION}")

## Use all the *.cpp files we found under this folder for the project Find better way for coomen cpp files
FILE(GLOB SRCS "*.cpp" "*/*.cpp")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    SET (project_BIN ${PROJECT_NAME})
else()
    SET (project_BIN ${PROJECT_NAME}.bin)
endif()

## Define the executable
add_executable(${project_BIN} ${SRCS} ${MAIN_common_SRCS} ${easylogging_SRCS} ${common_utils_SRCS} ${common_utils_SRCS_LOGGER} ${common_utils_communication_SRCS})

TARGET_LINK_LIBRARIES(${project_BIN} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES})
TARGET_LINK_LIBRARIES(${project_BIN} ${SDL2_NET_LIBRARIES} systemd )

set(MAINFONTSOURCE "${MAINFOLDER}/src/backend/loggerBackend.conf")
add_custom_command(TARGET ${project_BIN} POST_BUILD # Adds a post-build event to MyTest
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
                       "${MAINFONTSOURCE}"      # <--this is in-file
                       $<TARGET_FILE_DIR:${project_BIN}>)