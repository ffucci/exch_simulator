include(CTest)
include(GoogleTest)
enable_testing()

find_package(GTest REQUIRED)

add_executable(
    test_books
    main.cpp
    books/test_price_orders_container.cpp
    books/test_order_book.cpp
)

if(CODE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    message("${PROJECT_SOURCE_DIR}/lib/src")
    setup_target_for_coverage_gcovr_html(NAME coverage EXECUTABLE ./test_books BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/lib/src")
endif()


target_include_directories(test_books PUBLIC include)

target_link_libraries(
    test_books

    order_books
    GTest::gtest_main
)

gtest_discover_tests(test_books)