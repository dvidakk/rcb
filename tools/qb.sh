#!/bin/bash
set -e

# used to quick build a project from the root directory using cmake

# check if the build directory exists
# if not, create it
# if it does, remove it and create a new one if clean build is requested

# -c flag is used to clean the build directory
# -d flag is used to specify the build directory
# -h flag is used to display help
#
# Usage: qb.sh [-c] [-d build_dir] [-h]

# default build directory
BUILD_DIR="./bld"

# default clean flag
# 0 - do not clean
# 1 - clean
# 2 - clean and rebuild
CLEAN_FLAG=0

# default help flag
# 0 - do not display help
# 1 - display help
HELP_FLAG=0

mMAKEFLAGS="-j$(nproc) -l$(nproc)"

display_help() {
    echo "Usage: qb.sh [-c] [-d build_dir] [-h]"
    echo "  -c  clean the build directory"
    echo "  -d  specify the build directory"
    echo "  -h  display help"
    exit 0
}

clean_build_directory() {
    echo "Cleaning the build directory"
    rm -rf $BUILD_DIR
}

create_build_directory() {
    echo "Creating the build directory"
    mkdir $BUILD_DIR
}

# Argument parsing
while getopts "cd:h" opt; do
    case $opt in
        c) CLEAN_FLAG=1 ;;
        d) BUILD_DIR=$OPTARG ;;
        h) HELP_FLAG=1 ;;
        \?) echo "Invalid option: -$OPTARG" >&2 ;;
    esac
done

# Main logic
if [ $HELP_FLAG -eq 1 ]; then
    display_help
fi

if [ -d $BUILD_DIR ]; then
    if [ $CLEAN_FLAG -eq 1 ]; then
        clean_build_directory
        create_build_directory
    else
        echo "Build directory already exists"
    fi
else
    create_build_directory
fi

pushd ${BUILD_DIR}
  cmake ..
  make ${mMAKEFLAGS}
popd
