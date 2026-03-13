cmake_minimum_required(VERSION 3.20)
project(katana_comparison_drogon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Drogon CONFIG REQUIRED)

add_executable(katana_comparison_drogon main.cpp)
target_link_libraries(katana_comparison_drogon PRIVATE Drogon::Drogon)
