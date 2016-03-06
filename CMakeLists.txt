cmake_minimum_required(VERSION 2.8.4)
project(malloc)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

add_custom_target(malloc COMMAND make -C ${malloc_SOURCE_DIR}
        CLION_EXE_DIR=${PROJECT_BINARY_DIR})
