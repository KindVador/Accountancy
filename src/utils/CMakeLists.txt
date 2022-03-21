add_library(AccountancyUtils
        scopedtimer.hpp
        instrumentor.hpp
        )

target_link_libraries(AccountancyUtils PRIVATE Qt::Core)
target_include_directories(AccountancyUtils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")