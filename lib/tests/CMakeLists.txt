include(CTest)
include(GoogleTest)
enable_testing()

find_package(GTest REQUIRED)

add_executable(
    test_books
    main.cpp
    books/test_price_orders_container.cpp
)

if(CODE_COVERAGE)
    include(CodeCoverage.cmake)
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcov(NAME coverage EXECUTABLE ./bin/tests BASE_DIRECTORY ../coverage)
endif()


target_include_directories(test_books PUBLIC include)

target_link_libraries(
    test_books

    order_books
    GTest::gtest_main
)

gtest_discover_tests(test_books)