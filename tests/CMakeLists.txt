cmake_minimum_required(VERSION 3.20)

project(TestsAccountancy LANGUAGES CXX)

include_directories(../src/core)

find_package(Qt6 COMPONENTS Core Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib/Catch2/single_include/catch2)
add_library(Catch2 INTERFACE)
target_include_directories(Catch2 INTERFACE ${CATCH_INCLUDE_DIR})

add_executable(TestsAccountancy core/tst_account.cpp)
target_link_libraries(TestsAccountancy PRIVATE Catch2 Qt6::Core Qt6::Test AccountancyCore)

#include(CTest)
#catch_discover_tests(foo)
