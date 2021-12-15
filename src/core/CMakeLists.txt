add_subdirectory(encryption)

target_sources(KeyContainer PRIVATE commoninclude.cpp debugshowoptions.cpp kcdb.cpp keymapjsonengine.cpp main.cpp)

if(COMPILE_QML)
	target_sources(KeyContainer PRIVATE qmlimporter.cpp)
endif()
