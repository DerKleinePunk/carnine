message(STATUS "working on common dir")

ADD_SUBDIRECTORY(utils)

set(common_utils_SRCS ${common_utils_SRCS} ${SRCS_II} PARENT_SCOPE)
set(common_utils_SRCS_LOGGER ${common_utils_SRCS_LOGGER} PARENT_SCOPE)