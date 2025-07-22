find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/onnx)

set(ONNX_SRC ${PROJECT_SOURCE_DIR}/3rdparty/onnx/onnx.pb.cc)

set(ONNX_DPE_LIBS
    ${PROTOBUF_LIBRARY}
    absl::absl_check
)
