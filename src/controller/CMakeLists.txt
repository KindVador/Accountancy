set(NAME AccountancyController)

set(HEADER_FILES
        controller.hpp
)

set(SOURCE_FILES
        controller.cpp
)

add_library(${NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${NAME} PRIVATE Qt::Core Qt::Widgets AccountancyCore)
target_include_directories(${NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/..")