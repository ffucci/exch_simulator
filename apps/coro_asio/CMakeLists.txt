find_package(Boost)
find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
add_executable(coro_asio_sleep coro_asio_sleep.cpp)