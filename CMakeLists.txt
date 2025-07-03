cmake_minimum_required(VERSION 3.10.0)

project(yaccs VERSION 0.0.1)

add_compile_options(-g)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory(3rdparty/onnx)
add_subdirectory(yaccs)
