cmake_minimum_required(VERSION 3.24)
project(.)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_C_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -g -O3 -fPIC -march=native)

find_package(Vulkan REQUIRED)

execute_process(COMMAND "submodules/shaderc/utils/git-sync-deps")

set(BUILD_SHARED_LIBS OFF)
add_subdirectory("submodules/fmt")
add_subdirectory("submodules/glfw")
add_subdirectory("submodules/portaudio")
add_subdirectory("submodules/tinyobjloader")
add_subdirectory("submodules/freetype")

set(SPIRV_REFLECT_EXECUTABLE OFF CACHE BOOL "SPIRV-Reflect executable")
set(SPIRV_REFLECT_EXAMPLES OFF CACHE BOOL "SPIRV-Reflect examples")
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "SPIRV-Reflect static library")
add_subdirectory("submodules/spirv-reflect")

set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "Shaderc examples")
set(SHADERC_SKIP_TESTS ON CACHE BOOL "Shaderc tests")
set(SHADERC_SKIP_INSTALL ON CACHE BOOL "Shaderc install")
add_subdirectory("submodules/shaderc")

set(BUILD_SHARED_LIBS ON)
set(BUILD_TESTING OFF CACHE BOOL "Libsndfile testing")
set(BUILD_PROGRAMS OFF CACHE BOOL "Libsndfile programs")
set(BUILD_EXAMPLES OFF CACHE BOOL "Libsndfile exapmles")
add_subdirectory("submodules/libsndfile")


# Dynamo library files
file(GLOB_RECURSE INCLUDE "./src/*.hpp" "./src/*.h")
file(GLOB_RECURSE SOURCES "./src/*.cpp" "./src/*.c")

# Make Dynamo a shared library
add_library(Dynamo SHARED ${SOURCES} ${INCLUDE} ${STB_SOURCES})
target_include_directories(Dynamo PUBLIC "./src" "./submodules/stb")

# Link all libraries together
target_link_libraries(Dynamo fmt glfw Vulkan::Vulkan PortAudio sndfile tinyobjloader spirv-reflect-static shaderc)
