cmake_minimum_required(VERSION 3.20)

project(TestsAccountancy LANGUAGES CXX)

include_directories(../src)

find_package(Qt6 COMPONENTS Core Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(TestsAccountancy
               tst_dataimport.cpp)

add_test(NAME TestsAccountancy COMMAND TestsAccountancy)

find_library (
        CORE_LIB
        NAMES libAccountancyCore.a
        HINTS "${CMAKE_BINARY_DIR}/src/core"
        NO_DEFAULT_PATH
)
message(STATUS "CORE_LIB: [${CORE_LIB}]")
target_link_libraries(TestsAccountancy PRIVATE Qt6::Core Qt6::Test ${CORE_LIB})
