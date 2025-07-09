add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${ONNX_SRC}
)

target_link_libraries(${PROJECT_NAME}
    ${ONNX_DPE_LIBS}
)
