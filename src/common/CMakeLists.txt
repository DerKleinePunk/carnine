message(STATUS "working on common dir")

ADD_SUBDIRECTORY(utils)
ADD_SUBDIRECTORY(communication)
ADD_SUBDIRECTORY(messages)

set(common_utils_SRCS ${common_utils_SRCS} ${SRCS_II} PARENT_SCOPE)
set(common_utils_SRCS_LOGGER ${common_utils_SRCS_LOGGER} PARENT_SCOPE)
set(common_utils_communication_SRCS ${common_utils_communication_SRCS} PARENT_SCOPE)
set(common_messages_SRCS ${common_messages_SRCS} PARENT_SCOPE)