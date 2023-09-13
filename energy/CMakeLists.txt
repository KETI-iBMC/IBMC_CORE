cmake_minimum_required(VERSION 3.10)
project(EnergyCalc VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)
include_directories(lib/eigen)

# Add the library subdirectory
add_subdirectory(src)