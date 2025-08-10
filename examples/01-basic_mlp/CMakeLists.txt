include_directories(${cov_INCS})

add_executable(basic_mlp01
    main.cpp
)

target_link_libraries(basic_mlp01
    vulkan
)
