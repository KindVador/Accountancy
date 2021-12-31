cmake_minimum_required(VERSION 3.20)

project(TestsAccountancy LANGUAGES CXX)

include_directories(../src)

find_package(Qt6 COMPONENTS Core Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(TestsDataImport tst_dataimport.cpp)
add_test(NAME TestsDataImport COMMAND TestsDataImport)

add_executable(TestsTransaction tst_transaction.cpp)
add_test(NAME TestsTransaction COMMAND TestsTransaction)

add_executable(TestsCurrency tst_currency.cpp)
add_test(NAME TestsCurrency COMMAND TestsCurrency)

find_library (
        CORE_LIB
        NAMES libAccountancyCore.a
        HINTS "${CMAKE_BINARY_DIR}/src/core"
        NO_DEFAULT_PATH
)
message(STATUS "CORE_LIB: [${CORE_LIB}]")
target_link_libraries(TestsDataImport PRIVATE Qt6::Core Qt6::Test ${CORE_LIB})
target_link_libraries(TestsTransaction PRIVATE Qt6::Core Qt6::Test ${CORE_LIB})
target_link_libraries(TestsCurrency PRIVATE Qt6::Core Qt6::Test ${CORE_LIB})