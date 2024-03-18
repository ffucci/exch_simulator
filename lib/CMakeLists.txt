find_package(Boost)
find_package(Threads REQUIRED)
find_package(plf_list REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_library(ds_blocks
    src/lockless_queue.cpp
)

target_include_directories(ds_blocks PUBLIC include)

add_library(tcp_server
    src/asio_generic_server.cpp
    src/coro_asio_server.cpp
    src/server.cpp
)

target_include_directories(tcp_server PUBLIC include)
target_link_libraries(tcp_server ds_blocks)

add_library(order_books
    src/order_book.cpp
    src/price_orders_container.cpp
)

target_include_directories(order_books PUBLIC include)
target_link_libraries(order_books plf_list::plf_list)

add_subdirectory(tests)