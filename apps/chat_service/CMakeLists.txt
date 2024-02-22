find_package(Boost)
find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(chat_service chat_service.cpp)
target_link_libraries(chat_service tcp_server Threads::Threads)