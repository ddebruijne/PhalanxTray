cmake_minimum_required(VERSION 3.5)
project(Klonk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib/tray)
add_executable(PhalanxTray
	source/defines.h
	source/main.cpp
)

target_include_directories(PhalanxTray PUBLIC lib/tray)
target_link_libraries(PhalanxTray PUBLIC tray::tray)

configure_file(${CMAKE_CURRENT_LIST_DIR}/data/icon.ico ${CMAKE_BINARY_DIR}/icon.ico COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/data/icon.png ${CMAKE_BINARY_DIR}/icon.png COPYONLY)