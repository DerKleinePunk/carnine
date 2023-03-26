message(STATUS "working on messages dir")

FILE (GLOB_RECURSE SRCS "*.cpp")

set(common_messages_SRCS ${SRCS} PARENT_SCOPE)