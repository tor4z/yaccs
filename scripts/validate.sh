#!/bin/env bash

if [[ $# -eq 0 ]]; then
    echo "No spvasm file to validate"
    exit 1
fi


spirv-as $1 -o $1.spv &&\
spirv-val $1.spv &&\
spirv-cross $1.spv | highlight --syntax c -O xterm256 --style darkness
