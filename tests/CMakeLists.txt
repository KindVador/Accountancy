cmake_minimum_required(VERSION 3.20)

project(TestsAccountancy LANGUAGES CXX)

include_directories(../src/core)

find_package(Qt6 COMPONENTS Core Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prepare "Catch" library for other executables
add_subdirectory(../external_libs/Catch2 "${CMAKE_CURRENT_BINARY_DIR}/catch2")

set(TEST_SRC
        main.cpp
        core/tst_account.cpp
        core/tst_category.cpp
        core/tst_currency.cpp
        core/tst_dataimport.cpp
        core/tst_financialinstitution.cpp
        core/tst_owner.cpp
        core/tst_transaction.cpp
        core/tst_import_config.cpp
        )

add_executable(TestsAccountancy ${TEST_SRC})
target_link_libraries(TestsAccountancy PRIVATE Catch2::Catch2 Qt6::Core Qt6::Widgets AccountancyCore)

# copy tests data in the same folder than tests binary
add_custom_command(
        TARGET TestsAccountancy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/tests/data
        ${CMAKE_CURRENT_BINARY_DIR}/data)

include(CTest)
include(../external_libs/Catch2/contrib/Catch.cmake)
catch_discover_tests(TestsAccountancy)
