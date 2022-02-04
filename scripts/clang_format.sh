#!/bin/bash

declare -a script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

declare -a src_files=($script_dir/../main.cpp $script_dir/../include/*.hpp $script_dir/../src/*.cpp)

for s in "${src_files[@]}"; do
    clang-format -i $s
done

