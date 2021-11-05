cmake_minimum_required(VERSION 3.20)

project(
    Hue-Lights-2
    VERSION 0.1
    DESCRIPTION "Hue Lights 2"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)