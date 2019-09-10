# See the cmake tutorial: https://cmake.org/cmake-tutorial/
cmake_minimum_required(VERSION 3.14)
project(swipr)

set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )

include_directories(preprocessing)

add_executable(swipr
        preprocessing/roi.cpp
        preprocessing/roi.h preprocessing/keycodes.h)

target_link_libraries( swipr ${OpenCV_LIBS} )

# TODO: add testing here