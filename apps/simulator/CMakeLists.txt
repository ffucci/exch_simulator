find_package(Boost)
find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(simulator simulator.cpp)
add_executable(order_sender order_sender.cpp)