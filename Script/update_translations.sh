#!/bin/bash
# Author: Eduardo Mozart de Oliveira <eduardomozart182@gmail.com>

# This script finds all files with the .ts extension recursively
# from the current directory and then executes a command on each one.

# Get the directory of the script itself, regardless of where it's executed from.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Remove the '/Script' component from the end of the directory path.
SCRIPT_DIR="${SCRIPT_DIR%/Script}"

# Only update TS files of a specific language (e.g., pt_BR) if the first command-line argument is set.
if [ -n "$1" ]; then
	TS_LANG=$1
fi

echo "Starting recursive search for *.ts files in $SCRIPT_DIR..."

find "$SCRIPT_DIR" -name "*.ts" | while read -r TS_FILE; do
    if [[ "$TS_FILE" == *"Translations"* ]]; then
        # Check if TS_LANG is not set or if the current TS file matches the language.
        if [ -z "$TS_LANG" ] || [[ "$TS_FILE" == *"$TS_LANG"* ]]; then
            # Use 'sed' to remove the "/Resources/Translations/" part from the file name.
            # The 's' command stands for substitute. The pipes '|' are used as delimiters
            # instead of the more common '/' to avoid escaping the slashes in the path.
            PROJECT_PATH=$(echo $(dirname "$TS_FILE") | sed 's|/Resource/Translations||')

            lupdate $PROJECT_PATH -ts "$TS_FILE" -no-obsolete
        fi
    fi
done
