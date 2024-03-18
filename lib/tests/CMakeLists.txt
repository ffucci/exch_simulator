enable_testing()

find_package(GTest REQUIRED)

add_executable(
    test_books
    main.cpp
    books/test_price_orders_container.cpp
)

target_include_directories(test_books PUBLIC include)

target_link_libraries(
    test_books

    order_books
    gtest::gtest
)