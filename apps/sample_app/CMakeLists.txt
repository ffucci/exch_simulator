find_package(ZLIB REQUIRED)

add_executable(sample_app sample.c)
target_link_libraries(sample_app ZLIB::ZLIB)