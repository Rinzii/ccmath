#!/bin/bash
# Run the Python formatting script for the specified directory and extensions

python format.py ../include/ccmath --extensions .h,.hpp

# Check the exit code to display an appropriate message
if [ $? -ne 0 ]; then
    echo "Error: Failed to run the format script."
    exit 1
else
    echo "Format script ran successfully."
fi
