cmake_minimum_required(VERSION 3.10.0)

project(yaccs VERSION 0.0.1)

add_compile_options(-g)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}/${CMAKE_CURRENT_LIST_DIR}/3rdparty/cmake")

include(onnx)


add_subdirectory(yaccs)
