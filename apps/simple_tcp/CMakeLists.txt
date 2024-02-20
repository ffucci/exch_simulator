find_package(Boost)
find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(tcp_echo_server tcp_echo_server.cpp)
target_link_libraries(tcp_echo_server tcp_server)
