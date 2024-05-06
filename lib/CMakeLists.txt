find_package(Boost)
find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(../abseil-cpp)

add_library(ds_blocks
    src/lockless_queue.cpp
)

target_include_directories(ds_blocks PUBLIC include)

add_library(tcp_server
    src/asio_generic_server.cpp
    src/coro_asio_server.cpp
    src/orders_server.cpp
    src/server.cpp
)

add_library(order_books
    src/order_book.cpp
    src/fifo_matching.cpp
    src/price_orders_container.cpp
    src/types.cpp
)

if(CODE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags_to_target(order_books)
endif()

target_include_directories(order_books PUBLIC include)
target_link_libraries(order_books PUBLIC absl::btree)

target_include_directories(tcp_server PUBLIC include)
target_link_libraries(tcp_server ds_blocks order_books)


add_subdirectory(tests)