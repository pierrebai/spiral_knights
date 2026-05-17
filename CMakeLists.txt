cmake_minimum_required(VERSION 3.10)
project(Spiral_Knights)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the executable
add_executable(spiral_knights spiral_knights.cpp)

# Optional: Set compiler flags for optimization
if(MSVC)
    target_compile_options(spiral_knights PRIVATE /W4 /O2)
else()
    target_compile_options(spiral_knights PRIVATE -Wall -Wextra -O2)
endif()
