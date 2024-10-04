#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Script to build SpiroBot

# Define colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print colored output
print_color() {
    printf "%b%s%b\n" "$1" "$2" "${NC}"
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    print_color "${RED}" "Error: CMakeLists.txt not found. Please run this script from the project root directory."
    exit 1
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    print_color "${YELLOW}" "Creating build directory..."
    mkdir build
fi

# Navigate to build directory
cd build

# Clean build directory
print_color "${YELLOW}" "Cleaning build directory..."
rm -rf *

# Run CMake
print_color "${YELLOW}" "Running CMake..."
cmake ..

# Build the project
print_color "${YELLOW}" "Building SpiroBot..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    print_color "${GREEN}" "Build successful! You can now run ./SpiroBot"
else
    print_color "${RED}" "Build failed. Please check the output for errors."
    exit 1
fi

# Return to the original directory
cd ..

print_color "${GREEN}" "Build process completed."
