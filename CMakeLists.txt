cmake_minimum_required(VERSION 3.13)

set(APP_VERSION 3.1.10)

project(KeyContainer
	VERSION ${APP_VERSION}
	LANGUAGES CXX)

# Set compile options
set(COMPILE_QML ON CACHE BOOL "Compile Qml version" FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH $ENV{QT_CMAKE_PATH})

message(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

add_definitions(-DAPP_VERSION="${APP_VERSION}")
if(COMPILE_QML)
	message(STATUS "COMPILE_QML is enabled")
	find_package(
		Qt5
		CONFIG
		REQUIRED
		Core
		Gui
		Qml
        Quick)
	add_definitions(-DENABLE_QML)
else()
	message(STATUS "COMPILE_QML is NOT enabled")
	find_package(
		Qt5
		CONFIG
		REQUIRED
		Core
		Gui
		Widgets)
endif()

if(WIN32)
	# Force UTF-8 in MSVC
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
	# Add definitions for Windows rc file
	add_definitions(-DKEYCONTAINER_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR})
	add_definitions(-DKEYCONTAINER_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR})
	add_definitions(-DKEYCONTAINER_VERSION_PATCH=${CMAKE_PROJECT_VERSION_PATCH})
	add_definitions(-DKEYCONTAINER_VERSION_BUILD=1)
	add_definitions(-DKEYCONTAINER_VERSION_STRING="${PROJECT_VERSION}")
elseif(UNIX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
endif()

include_directories(${CMAKE_PREFIX_PATH}/../../include)
include_directories(${CMAKE_PREFIX_PATH}/../../include/QtCore)
include_directories(${CMAKE_PREFIX_PATH}/../../include/QtGui)
include_directories(${CMAKE_PREFIX_PATH}/../../include/QtWidgets)
include_directories(src/)
include_directories(src/core)
include_directories(src/utils)

if(WIN32)
    add_executable(KeyContainer WIN32)
else()
    add_executable(KeyContainer)
endif()

add_subdirectory(src)

if(COMPILE_QML)
    target_link_libraries(${PROJECT_NAME} optimized Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick)
else()
    target_link_libraries(${PROJECT_NAME} optimized Qt5::Core Qt5::Gui Qt5::Widgets)
endif()

if(UNIX)
    target_link_libraries(${PROJECT_NAME} optimized X11)
endif()
