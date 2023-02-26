message(STATUS "working on utils dir")

FILE (GLOB_RECURSE SRCS "CommandLineArgs.cpp")
FILE (GLOB_RECURSE SRCS_II "LogNetDispatcher.cpp" "UdpLogClient.cpp" "time-util.cpp")

set(common_utils_SRCS ${SRCS} PARENT_SCOPE)
set(common_utils_SRCS_LOGGER ${SRCS_II} PARENT_SCOPE)