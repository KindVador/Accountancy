cmake_minimum_required(VERSION 3.20)

project(TestsAccountancy LANGUAGES CXX)

include_directories(../src/core)

find_package(Qt6 COMPONENTS Core Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prepare "Catch" library for other executables
add_subdirectory(../lib/Catch2 "${CMAKE_CURRENT_BINARY_DIR}/catch2")

add_executable(TestsAccountancy core/tst_account.cpp)
target_link_libraries(TestsAccountancy PRIVATE Catch2::Catch2 Qt6::Core Qt6::Test AccountancyCore)

include(CTest)
include(../lib/Catch2/contrib/Catch.cmake)
catch_discover_tests(TestsAccountancy)
