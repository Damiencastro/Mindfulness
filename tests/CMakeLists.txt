cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/repositories
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services/application
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services/infrastucture
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

set(CMAKE_AUTOMOC ON)

# Unit Test Executable
add_executable(test_ApplicationRepository
    unit/test_ApplicationRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/ApplicationRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/Application.cpp
)

target_link_libraries(test_ApplicationRepository Qt6::Test Qt6::Core)
add_test(NAME ApplicationRepository COMMAND test_ApplicationRepository)

add_executable(test_GameSession
    unit/test_GameSession.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/GameSession.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TimeSetDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/WarningDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/services/utils/ProcessUtils.cpp
)
target_link_libraries(test_GameSession Qt6::Test Qt6::Core Qt6::Widgets)
add_test(NAME GameSession COMMAND test_GameSession)