cmake_minimum_required(VERSION 3.16)
project(Mindfulness CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/monitoring
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# Collect source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.h
)

# Resources
set(RESOURCES resources/resources.qrc)

# Main executable
add_executable(Mindfulness
    ${SOURCES}
    ${RESOURCES}
)

target_link_libraries(Mindfulness PRIVATE
    Qt6::Core
    Qt6::Widgets
    Psapi
    User32
)

# Testing (optional - only if BUILD_TESTS is ON)
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()