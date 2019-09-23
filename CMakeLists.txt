# See the cmake tutorial: https://cmake.org/cmake-tutorial/
cmake_minimum_required(VERSION 3.14)
project(swipr)

set(CMAKE_CXX_STANDARD 14)

option(DEBUG
        "Turn on internal debug output" true)
if (DEBUG)
    add_definitions(-DDBG_OUTPUT)
endif ()

find_package( OpenCV REQUIRED )

include_directories(preprocessing, core)

add_executable(calibrate_test
        preprocessing/calibrate.cpp
        preprocessing/calibrate.h core/keycodes.h core/data.h core/debug.h core/data.cpp)

target_link_libraries(calibrate_test ${OpenCV_LIBS})

# TODO: add testing here