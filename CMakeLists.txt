cmake_minimum_required(VERSION 3.16)

project(
    hue-qt
    VERSION 1.0
    DESCRIPTION "Hue-QT"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-unused-parameter)
add_compile_options(-Wall -Wextra -pedantic -Werror )

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)