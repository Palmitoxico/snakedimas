cmake_minimum_required(VERSION 3.1)
project(snakedimas)

# This project requires C++14
set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Docopt library
add_subdirectory(third_party/docopt.cpp docopt_build)

set(snakedimas_src_files)

# Display source files
list(APPEND snakedimas_src_files src/display/display.cpp)

# Sound effects source files
list(APPEND snakedimas_src_files src/serialize/serialize.cpp)

# Input source files
list(APPEND snakedimas_src_files src/input/input.cpp)

# Model source files
list(APPEND snakedimas_src_files src/model/model.cpp)

# Log and debug messages source files
list(APPEND snakedimas_src_files src/utils/msglog.cpp)

# Network source files
list(APPEND snakedimas_src_files src/net/net.cpp)

add_executable(snakedimas src/main.cpp ${snakedimas_src_files})
target_link_libraries(snakedimas ${CURSES_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} docopt_s)

add_executable(snakeserver src/server.cpp ${snakedimas_src_files})
target_link_libraries(snakeserver ${CURSES_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} docopt_s)
