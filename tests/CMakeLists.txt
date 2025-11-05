cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/monitoring
    ${CMAKE_SOURCE_DIR}/src/ui
)

set(CMAKE_AUTOMOC ON)

# Unit Test Executable
add_executable(test_GameList
    unit/test_GameList.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GameList.cpp
)

target_link_libraries(test_GameList Qt6::Test Qt6::Core)
add_test(NAME GameList COMMAND test_GameList)

add_executable(test_GameSession
    unit/test_GameSession.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GameSession.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TimeSetDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/WarningDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/monitoring/ProcessUtils.cpp
)
target_link_libraries(test_GameSession Qt6::Test Qt6::Core Qt6::Widgets)
add_test(NAME GameSession COMMAND test_GameSession)