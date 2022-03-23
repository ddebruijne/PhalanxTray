cmake_minimum_required(VERSION 3.5)
project(Klonk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/modules)
set(MACOSX_MAKE_BUNDLE ON)

if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)
elseif(UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

set(PHALANXTRAY_EXE_FILES
	source/ContentMode/ContentModeBase.cpp
	source/ContentMode/ContentModeFFXIV.cpp
	source/ContentMode/ContentModeTime.cpp
	source/savedata.cpp
	source/main.cpp
	lib/loguru/loguru.cpp
)

if(WIN32)
	set(PHALANXTRAY_EXE_FILES ${PHALANXTRAY_EXE_FILES} source/Platform/windows.cpp)
	configure_file(${CMAKE_CURRENT_LIST_DIR}/data/icon.ico ${CMAKE_BINARY_DIR}/bin/icon.ico COPYONLY)
	add_executable(PhalanxTray ${PHALANXTRAY_EXE_FILES} data/appicon.rc)
elseif(UNIX)
	if(APPLE AND ${MACOSX_MAKE_BUNDLE})
		set(PHALANXTRAY_EXE_FILES ${PHALANXTRAY_EXE_FILES} source/Platform/darwin.mm)
		set(MACOSX_BUNDLE_ICON_FILE icon.icns)
		set(MACOSX_BUNDLE_GUI_IDENTIFIER donk.phalanxtray)
		set(APP_ICON_MACOSX 
			${CMAKE_CURRENT_LIST_DIR}/data/icon.icns
			${CMAKE_CURRENT_LIST_DIR}/data/icon.png
		)
		set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES
			MACOSX_PACKAGE_LOCATION "Resources")
		add_executable(PhalanxTray MACOSX_BUNDLE ${PHALANXTRAY_EXE_FILES} ${MACOSX_BUNDLE_INFO_PLIST} ${APP_ICON_MACOSX})
	else()
		configure_file(${CMAKE_CURRENT_LIST_DIR}/data/icon.png ${CMAKE_BINARY_DIR}/bin/icon.png COPYONLY)
		add_executable(PhalanxTray ${PHALANXTRAY_EXE_FILES})
	endif()
endif()

target_include_directories(PhalanxTray PRIVATE include)

add_subdirectory(lib/tray)
add_subdirectory(lib/serial)
add_subdirectory(lib/fmt)
add_subdirectory(lib/asio)

target_include_directories(PhalanxTray PUBLIC lib/tray)
target_link_libraries(PhalanxTray PUBLIC tray::tray)

target_include_directories(PhalanxTray PUBLIC lib/serial/include)
target_link_libraries(PhalanxTray PUBLIC serial)

target_include_directories(PhalanxTray PUBLIC lib/fmt/include)
target_link_libraries(PhalanxTray PUBLIC fmt)

target_link_libraries(PhalanxTray PUBLIC asio::asio)

target_include_directories(PhalanxTray PUBLIC lib/loguru)