cmake_minimum_required(VERSION 3.20)

project(RayTracer VERSION 1.0 LANGUAGES C CXX)

# only use clang or clang++
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Compiler ID: " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "Debug Flags" ${CMAKE_CXX_FLAGS_DEBUG})
message(STATUS "Release Flags" ${CMAKE_CXX_FLAGS_RELEASE})

# compile flags
add_compile_options(-Wall -Wextra)
add_compile_options(-Wnon-virtual-dtor)
add_compile_options(-Wreorder)
add_compile_options(-Wsometimes-uninitialized)
add_compile_options(-Wunused-variable)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g)
else()
  add_compile_options(-O3 -march=native -ffast-math -fno-finite-math-only)
endif()

# use stdc++ instead of libstdc++
add_compile_options(-stdlib=libc++)
add_link_options(-stdlib=libc++ -lc++abi)


include_directories("${PROJECT_BINARY_DIR}/include")

add_executable(raytracer)
target_sources(raytracer
  PRIVATE
  "src/main.cc"
)

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
  set(HAVE_OPENMP ON)
endif()
if(HAVE_OPENMP)
  target_compile_definitions(raytracer PUBLIC HAVE_OPENMP=1)
  target_link_libraries(raytracer PUBLIC OpenMP::OpenMP_CXX)
  target_compile_options(raytracer PRIVATE -fopenmp)
  target_link_options(raytracer PRIVATE -fopenmp)
endif()
