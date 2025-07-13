file(GLOB_RECURSE SRCS *.cpp)

add_executable(${PROJECT_NAME}
    ${SRCS}
    ${ONNX_SRC}
)

target_link_libraries(${PROJECT_NAME}
    ${ONNX_DPE_LIBS}
)
