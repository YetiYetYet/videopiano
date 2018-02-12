cmake_minimum_required(VERSION 3.8)
project(Beladj)

set(CMAKE_CXX_STANDARD 17)

find_package( OpenCV REQUIRED )

set(SOURCE_FILES main.cpp)
add_executable(Beladj ${SOURCE_FILES})
target_link_libraries( Beladj ${OpenCV_LIBS} )

#External libraries
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(Beladj ${SFML_LIBRARIES})