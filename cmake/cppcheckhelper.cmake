find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)

add_custom_target(
        cppcheck warning,style,performance,information,unusedFunction
        COMMAND ${CMAKE_CXX_CPPCHECK}
        -DELPP_THREAD_SAFE
        -DELPP_CXX11
        --project=compile_commands.json
        -i${CMAKE_SOURCE_DIR}/src/common/easylogging
        --std=c++11
        --enable=warning,performance,portability,information
        --suppress-xml=${CMAKE_SOURCE_DIR}/tools/cppcheck/CppCheckSuppressions.xml
        --verbose
        --template=gcc
        --library=${CMAKE_SOURCE_DIR}/tools/cppcheck/sdl.cfg
)
