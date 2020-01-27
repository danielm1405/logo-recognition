cmake_minimum_required(VERSION 3.15)
project(POBR)

set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )

include_directories(include)
add_executable(POBR src/main.cpp include/Segment.h include/Utils.h include/LogoRecognizer.h src/LogoRecognizer.cpp src/Segment.cpp)
target_link_libraries( POBR ${OpenCV_LIBS} )
