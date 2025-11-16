#!/bin/bash

# Usage: ./collect_code.sh /path/to/folder output.txt
# Example: ./collect_code.sh ./src all_code.txt
# sh collect_code.sh ./folder output.txt

FOLDER="${1:-.}"                 # Default to current directory if not provided
OUTPUT_FILE="${2:-all_code.txt}" # Default output file name

# Clear or create output file
> "$OUTPUT_FILE"

# Loop through all files, skipping node_modules
find "$FOLDER" -type f | grep -Ev "/node_modules/|/package-lock\.json$" | while read -r FILE; do
  echo "==========================================" >> "$OUTPUT_FILE"
  echo "File: $FILE" >> "$OUTPUT_FILE"
  echo "==========================================" >> "$OUTPUT_FILE"
  cat "$FILE" >> "$OUTPUT_FILE"
  echo "\n\n" >> "$OUTPUT_FILE"
done

echo "✅ Code from all files in '$FOLDER' has been saved to '$OUTPUT_FILE'"