include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_PREFIX_PATH}/../../include/QtTest)

target_sources(KeyContainerTest PRIVATE maintest.cpp testencryption.h testencryption.cpp)
enable_testing()
add_test(KeyContainerTest KeyContainerTest -xml -o KeyContainerTest.xml)

target_link_libraries(KeyContainerTest Qt5::Core Qt5::Test)
