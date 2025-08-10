file(GLOB_RECURSE SRCS *.cpp)

include_directories(${flags_INCS})


add_executable(${PROJECT_NAME}
    ${SRCS}
    ${ONNX_SRC}
)

target_link_libraries(${PROJECT_NAME}
    ${ONNX_DPE_LIBS}
)
