#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <file> <directory>"
    exit 1
fi

source_file="$1"
target_directory="$2"

if [ ! -f "$source_file" ]; then
    echo "Error: Source file '$source_file' does not exist."
    exit 1
fi

if [ ! -d "$target_directory" ]; then
    echo "Error: Target directory '$target_directory' does not exist."
    exit 1
fi

find "$target_directory" -type f -name "zsim_hooks.h" | while read -r file; do
    echo "Replacing: $file"
    cp "$source_file" "$file"
done

echo "Replacement completed."


