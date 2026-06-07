cmake_minimum_required(VERSION 3.20)
project(katana_comparison_drogon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Drogon CONFIG REQUIRED)

add_executable(katana_comparison_drogon main.cpp)
target_link_libraries(katana_comparison_drogon PRIVATE Drogon::Drogon)

# Benchmark fairness: match KATANA's -O3 -march=native -mtune=native + IPO on the
# request-handling translation unit so app code is optimized identically.
# (libdrogon/libtrantor are the stock distro build, not rebuilt with -march=native,
# so this is "app-code optimized, vendor lib stock" — documented in README.)
target_compile_options(katana_comparison_drogon PRIVATE -O3 -march=native -mtune=native)
set_target_properties(katana_comparison_drogon PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
