cmake_minimum_required(VERSION 3.13)

set(APP_VERSION 3.2.3)

project(KeyContainer
	VERSION ${APP_VERSION}
	LANGUAGES CXX)

message(STATUS "CMake version: " ${CMAKE_VERSION})
add_definitions(-DUSE_CMAKE -DCMAKE_VERSION="${CMAKE_VERSION}")
# Set compile options
set(COMPILE_QML OFF CACHE BOOL "Compile Qml version" FORCE)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
        Quick
        Test)
	add_definitions(-DENABLE_QML)
else()
	message(STATUS "COMPILE_QML is NOT enabled")
	find_package(
		Qt5
		CONFIG
		REQUIRED
		Core
		Gui
        Widgets
        Test)
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

add_executable(KeyContainerTest)

add_subdirectory(src)
add_subdirectory(test)

if(COMPILE_QML)
    target_link_libraries(${PROJECT_NAME} optimized Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick)
else()
    if(UNIX)
        target_link_libraries(${PROJECT_NAME} optimized Qt5::Core Qt5::Gui Qt5::Widgets X11)
    else()
        target_link_libraries(${PROJECT_NAME} optimized Qt5::Core Qt5::Gui Qt5::Widgets)
    endif()

endif()

# Get git info
find_package(Git QUIET)
if(GIT_FOUND)
    set(GIT_COMMIT_TIME "")
    set(GIT_COMMIT "")
    set(GIT_COMMIT_LONG "")

    # GIT_BRANCH   = $$system(git rev-parse --abbrev-ref HEAD)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} show --oneline --format=\"%cd\" --date=short -s HEAD
        OUTPUT_VARIABLE GIT_COMMIT_TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} show --oneline --format=\"%h\" -s HEAD
        OUTPUT_VARIABLE GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} show --oneline --format=\"%H\" -s HEAD
        OUTPUT_VARIABLE GIT_COMMIT_LONG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_definitions(-DAPP_COMMIT_TIME=${GIT_COMMIT_TIME})
    add_definitions(-DAPP_COMMIT=${GIT_COMMIT})
    add_definitions(-DAPP_COMMIT_LONG=${GIT_COMMIT_LONG})
    message(STATUS "Git commit time:"      ${GIT_COMMIT_TIME})
    message(STATUS "Git commit:"           ${GIT_COMMIT})
    message(STATUS "Git commit full name:" ${GIT_COMMIT_LONG})
endif()
