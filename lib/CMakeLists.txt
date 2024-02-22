find_package(Boost)
find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_library(tcp_server
    src/asio_generic_server.cpp
    src/server.cpp
)

# add_library(tcp_server::processors ALIAS tcp_server)

target_include_directories(tcp_server PUBLIC include)