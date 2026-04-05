cmake_minimum_required(VERSION 3.10)
project(chip8_emulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include directories
include_directories(include ${SDL2_INCLUDE_DIRS})

# Add executable
add_executable(chip8_emulator
    src/main.cpp
    src/chip8.cpp
)

# Link SDL2
target_link_libraries(chip8_emulator ${SDL2_LIBRARIES})

# For Linux, we need to link against additional libraries
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(chip8_emulator m pthread)
endif()