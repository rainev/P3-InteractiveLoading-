cmake_minimum_required(VERSION 3.16)

project(my-sfml-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# If you installed SFML via package manager (apt, brew, vcpkg, etc.)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(main
    src/main.cpp
    src/PingPong.cpp
    src/AssetLoader.cpp
)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${CMAKE_BINARY_DIR}/assets
)

target_link_libraries(main PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Optional: On Windows, copy SFML DLLs next to the exe (leave out if not needed)
if (WIN32)
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Remember to place SFML DLLs beside the executable."
    )
endif()