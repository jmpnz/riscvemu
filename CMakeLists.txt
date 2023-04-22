cmake_minimum_required(VERSION 3.2)

project(riscvemu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
# Useful debug options for the default build
# lots of warnings and errors.
add_compile_options(-g -Wall -Wextra -Werror -pedantic -Wno-sign-compare)

include_directories("${PROJECT_SOURCE_DIR}/include")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/lib")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/bin")

# build the test executable
find_package(doctest REQUIRED)

# Make test executable
add_executable(riscvemu-tests tests/main.cpp src/lib/Instructions.cpp
  src/lib/Machine.cpp)
target_compile_features(riscvemu-tests PRIVATE cxx_std_17)
target_link_libraries(riscvemu-tests PRIVATE doctest::doctest)# build the main riscvemu executable

# build the binary
add_executable(riscvemu src/bin/main.cpp)
target_include_directories(riscvemu PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(riscvemu libriscvemu)

target_link_libraries(libriscvemu ${extra_libs})

# setup clang-tidy
# clang-tidy
if (NOT DEFINED CLANG_TIDY_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_TIDY_BIN
            NAMES clang-tidy clang-tidy-12)
endif()
if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "riscvemu/main couldn't find clang-tidy.")
else()
    # Output compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    message(STATUS "riscvemu/main found clang-tidy at ${CLANG_TIDY_BIN}")
endif()
