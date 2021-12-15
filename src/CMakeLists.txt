add_subdirectory(core)
add_subdirectory(resource)
add_subdirectory(utils)

if(COMPILE_QML)
	add_subdirectory(qml)
else()
	add_subdirectory(gui)
endif()
