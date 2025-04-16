# Add source files
set(SOURCES
    main.cpp
    bot/bot.cpp
    server/server.cpp
    utils/file_properties.cpp
    utils/custom_dl.cpp
    vars/vars.cpp
)

# Add header files
set(HEADERS
    bot/bot.hpp
    server/server.hpp
    utils/file_properties.hpp
    utils/custom_dl.hpp
    vars/vars.hpp
)

# Create executable
add_executable(tglink2aria2 ${SOURCES} ${HEADERS})

# Set include directories
target_include_directories(tglink2aria2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Link libraries
target_link_libraries(tglink2aria2
    PRIVATE
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        httplib::httplib
        tgbot-cpp::tgbot-cpp
        dotenv::dotenv
)

# Install targets
install(TARGETS tglink2aria2
    RUNTIME DESTINATION bin
) 