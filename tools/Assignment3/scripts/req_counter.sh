#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <directory>"
  exit 1
fi

if [ ! -d "$1" ]; then
  echo "Error: $1 is not a directory"
  exit 1
fi

for file in "$1"/*; do
  if [ -f "$file" ] && [[ $(basename "$file") != "labdata.csv" && $(basename "$file") != "summary.txt" ]]; then
    line_count=$(wc -l < "$file")
    echo "trace: $file - #requests: $line_count"
  fi
done
