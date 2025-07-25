#!/bin/env bash

if [[ $# -eq 0 ]]; then
    echo "No spvasm file to validate"
    exit 1
fi


spirv-as $1 --target-env vulkan1.0 -o $1.spv &&\
spirv-val --target-env vulkan1.0 $1.spv &&\
spirv-cross --vulkan-semantics $1.spv | highlight --syntax c -O xterm256 --style darkness
