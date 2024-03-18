find_package(Boost)
find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(simulator simulator.cpp)

target_link_libraries(simulator tcp_server)

add_executable(order_sender order_sender.cpp)

add_executable(prod_cons prod_cons.cpp)
target_link_libraries(prod_cons ds_blocks)
