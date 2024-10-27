cmake_minimum_required(VERSION 3.29)

project(json_eval VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
        -Werror

        -Wall
        -Wextra
        -Wpedantic

        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Wctor-dtor-privacy
        -Wenum-compare
        -Wfloat-equal
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wredundant-decls
        -Wsign-conversion
        -Wsign-promo
)

option(COVERAGE "Enable coverage reporting" OFF)
option(BUILD_TESTS "Build unit tests" ON)

include_directories(include)

add_executable(json_eval
        src/main.cpp
        src/json.cpp
        src/parser.cpp
)

if (BUILD_TESTS)
    find_package(GTest REQUIRED)

    if (UNIX)
        find_package(Threads REQUIRED)
    endif ()

    add_executable(unit_tests
            tests/main.cpp
            tests/json_tests.cpp
            tests/parse_tests.cpp
            tests/path_tests.cpp
            src/json.cpp
            src/parser.cpp
    )

    target_link_libraries(
            unit_tests
            GTest::GTest
            GTest::Main
            Threads::Threads
    )

    enable_testing()

    add_test(NAME unit_tests COMMAND unit_tests)

    if (COVERAGE)
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            message(STATUS "Enabling coverage reporting")
            target_compile_options(unit_tests PUBLIC --coverage -O0 -g)
            target_link_libraries(unit_tests gcov)
        endif ()
    endif ()

    if (UNIX)
        set(SOURCE_JSON_DIR "${CMAKE_SOURCE_DIR}/tests/data")
        set(TARGET_JSON_DIR "${CMAKE_BINARY_DIR}/test_data")

        if (NOT EXISTS ${TARGET_JSON_DIR})
            message(STATUS "Creating symlink for JSON test files from ${SOURCE_JSON_DIR} to ${TARGET_JSON_DIR}")
            execute_process(
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SOURCE_JSON_DIR} ${TARGET_JSON_DIR}
            )
        else ()
            message(STATUS "Symlink for JSON test files already exists")
        endif ()
    endif ()
endif ()
