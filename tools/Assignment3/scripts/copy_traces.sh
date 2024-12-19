#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <max_num_requests> <source_directory> <destination_directory>"
    exit 1
fi

NUM_REQ=$1
SOURCE_DIR=$2
DEST_DIR=$3
APPS_DIR=$3

if ! [[ "$NUM_REQ" =~ ^[0-9]+$ ]] || [ "$NUM_REQ" -le 0 ]; then
    echo "Error: The number of requests must be a positive integer."
    exit 1
fi

if [ ! -d "$SOURCE_DIR" ]; then
    echo "Error: Source directory '$SOURCE_DIR' does not exist."
    exit 1
fi

mkdir -p "$DEST_DIR"

for FILE in "$SOURCE_DIR"/*.trc; do
    if [ -f "$FILE" ]; then
        BASENAME=$(basename "$FILE")
        FOLDER_NAME="${BASENAME%.trc}"
        mkdir -p "$DEST_DIR/$FOLDER_NAME"
        cp "$FILE" "$DEST_DIR/$FOLDER_NAME/trace_C0.trc.shared"
    fi
done

for TRC in "$APPS_DIR"/*/trace_C0.trc.shared; do
    if [ -f "$TRC" ]; then
        TMP_FILE="${FILE}.tmp"
        # awk -v num_req="$NUM_REQ" 'NR == 1 { $1 = 0; print $0; next } NR <= num_req { print $0 }' "$TRC" > "$TMP_FILE"
        awk -v num_req="$NUM_REQ" 'NR <= num_req { $1 = 0; print }' "$TRC" > "$TMP_FILE"
        mv "$TMP_FILE" "$TRC"
    fi
done

echo "Traces copied into the destination directory with the Octopus-supported hierarchy and modified!"
