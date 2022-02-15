#!/bin/bash

### Instructions:
# Run as ./clang_format.sh --test to test if all source files
# are properly formatted
# Run with any other way or with no arguments to format them
### Requirements:
# clang-format

which clang-format > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo -e "ERROR: clang-format is not installed\n"\
        "Please install it to run this script."
    exit 1
fi

script_dir=`cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd`
declare -a src_files=($script_dir/../main.cpp $script_dir/../include/*.hpp $script_dir/../src/*.cpp)
if [ "$*" == --test ]; then
    tmp_src_file=`mktemp`
    for s in "${src_files[@]}"; do
        clang-format "$s" > $tmp_src_file
        if [[ `diff $s $tmp_src_file` != "" ]]; then
            echo -e "This repo uses clang spacing standards.\n"\
                "Please run ./scripts/clang_format.sh to format "\
                "all source files. Exiting..."
            rm $tmp_src_file
            exit 1
        fi
    done
    rm $tmp_src_file
else
    for s in "${src_files[@]}"; do
        clang-format -i "$s" 
    done
fi
