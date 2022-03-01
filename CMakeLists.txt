cmake_minimum_required(VERSION 3.5)
project(Klonk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup")
endif()

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

set(PHALANXTRAY_EXE_FILES
	source/ContentMode/ContentModeTime.cpp
	source/savedata.cpp
	source/main.cpp
)

if(WIN32)
	add_executable(PhalanxTray ${PHALANXTRAY_EXE_FILES} data/appicon.rc)
else()
	add_executable(PhalanxTray ${PHALANXTRAY_EXE_FILES})
endif()

target_include_directories(PhalanxTray PRIVATE include)

configure_file(${CMAKE_CURRENT_LIST_DIR}/data/icon.ico ${CMAKE_BINARY_DIR}/bin/icon.ico COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/data/icon.png ${CMAKE_BINARY_DIR}/bin/icon.png COPYONLY)

add_subdirectory(lib/tray)
add_subdirectory(lib/serial)
add_subdirectory(lib/fmt)

target_include_directories(PhalanxTray PUBLIC lib/tray)
target_link_libraries(PhalanxTray PUBLIC tray::tray)

target_include_directories(PhalanxTray PUBLIC lib/serial/include)
target_link_libraries(PhalanxTray PUBLIC serial)

target_include_directories(PhalanxTray PUBLIC lib/fmt/include)
target_link_libraries(PhalanxTray PUBLIC fmt)