cmake_minimum_required(VERSION 3.15)
project(TGlinkToAria2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)

# Find required packages
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(httplib REQUIRED)
find_package(tgbot-cpp REQUIRED)
find_package(dotenv REQUIRED)
find_package(CURL REQUIRED)

# Add subdirectories
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_DOCS)
    add_subdirectory(docs)
endif()

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Add source files
target_sources(${PROJECT_NAME} PRIVATE
    src/vars/vars.cpp
    src/utils/file_properties.cpp
    src/utils/byte_streamer.cpp
    src/server/server.cpp
    src/utils/aria2_client.cpp
)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    CURL::libcurl
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    Threads::Threads
)

# Add compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
) 