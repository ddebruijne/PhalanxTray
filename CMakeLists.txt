cmake_minimum_required(VERSION 3.5)
project(Klonk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup")
endif()

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_LIST_DIR}/bin>)
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
endif()

add_subdirectory(lib/tray)
add_executable(PhalanxTray
	source/main.cpp
)

target_include_directories(PhalanxTray PUBLIC lib/tray)
target_link_libraries(PhalanxTray PUBLIC tray::tray)

configure_file(${CMAKE_CURRENT_LIST_DIR}/data/icon.ico ${CMAKE_CURRENT_LIST_DIR}/bin/icon.ico COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/data/icon.png ${CMAKE_CURRENT_LIST_DIR}/bin/icon.png COPYONLY)