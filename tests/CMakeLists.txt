# Add test source files
set(TEST_SOURCES
    bot_test.cpp
    server_test.cpp
    utils_test.cpp
)

# Create test executable
add_executable(tglink2aria2_tests ${TEST_SOURCES})

# Set include directories
target_include_directories(tglink2aria2_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(tglink2aria2_tests
    PRIVATE
        tglink2aria2
)

# Add tests
add_test(NAME bot_test COMMAND tglink2aria2_tests --gtest_filter=BotTest.*)
add_test(NAME server_test COMMAND tglink2aria2_tests --gtest_filter=ServerTest.*)
add_test(NAME utils_test COMMAND tglink2aria2_tests --gtest_filter=UtilsTest.*) 