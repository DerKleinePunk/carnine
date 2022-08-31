message(STATUS "working on communication dir")

FILE (GLOB_RECURSE SRCS "*.cpp")

set(common_utils_communication_SRCS ${SRCS} PARENT_SCOPE)