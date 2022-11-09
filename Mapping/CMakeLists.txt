# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Inlcude build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project and C/C++ standard
project(Mapping C CXX ASM)
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD 17)

# Create pico-sdk subdirectory in project for libraries
pico_sdk_init()

add_executable(${PROJECT_NAME}
        mapping.c
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# pull in common dependencies
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
)