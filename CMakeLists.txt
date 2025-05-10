cmake_minimum_required(VERSION 3.20)
project(subtitle_converter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ────────────────────────────────────────────────────────────────
# Пути к заголовкам (включая catch2/catch.hpp)
# ────────────────────────────────────────────────────────────────
include_directories(${CMAKE_SOURCE_DIR}/include)

# ────────────────────────────────────────────────────────────────
#  Библиотека с реализациями форматов
# ────────────────────────────────────────────────────────────────
file(GLOB LIB_SOURCES
        src/Subtitle.cpp
        src/SRTSubtitle.cpp
        src/SAMISubtitle.cpp
        src/ASSSubtitle.cpp
        src/VTTSubtitle.cpp
)
add_library(subtitle_lib ${LIB_SOURCES})
target_include_directories(subtitle_lib PUBLIC include)

# ────────────────────────────────────────────────────────────────
#  Консольный конвертер
# ────────────────────────────────────────────────────────────────
add_executable(converter_subs src/main.cpp)
target_link_libraries(converter_subs PRIVATE subtitle_lib)

# ────────────────────────────────────────────────────────────────
#  Юнит-тесты (Catch2 header-only)
# ────────────────────────────────────────────────────────────────
enable_testing()

add_executable(tests_runner tests/tests.cpp)
target_link_libraries(tests_runner PRIVATE subtitle_lib)
add_test(NAME subtitle_tests COMMAND tests_runner)
