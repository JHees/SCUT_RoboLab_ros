cmake_minimum_required(VERSION 2.8)
set(CMAKE_BUILD_TYPE Release)

project(SCUT_RoboLab_ros)

add_compile_options(-std=c++11)

find_package(OpenCV 3.0 REQUIRED)

add_executable(SCUT_RoboLab_ros SCUT_RoboLab_ros.cpp)
target_link_libraries(SCUT_RoboLab_ros ${OpenCV_LIBRARIES})