#!/bin/bash

# This script formats the spacings in your code according
# to clang's standards

which clang-format > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo -e "ERROR: clang-format is not installed\n"\
        "Please install it to run this script."
    exit 1
fi

script_dir=`cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd`
declare -a src_files=($script_dir/../main.cpp $script_dir/../include/*.hpp $script_dir/../src/*.cpp)
for s in "${src_files[@]}"; do
    clang-format -i $s
done
