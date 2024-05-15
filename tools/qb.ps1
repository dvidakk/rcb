# used to quick build a project from the root directory using cmake

# check if the build directory exists
# if not, create it
# if it does, remove it and create a new one if clean build is requested

# -c flag is used to clean the build directory
# -d flag is used to specify the build directory
# -h flag is used to display help
#
# Usage: qb.ps1 [-c] [-d build_dir] [-h]

param(
    [switch]$c,
    [string]$d,
    [switch]$h
)

# default build directory
$BUILD_DIR = "./bld"

if ($d) {
    $BUILD_DIR = $d
}

function display_help {
    Write-Host "Usage: qb.ps1 [-c] [-d build_dir] [-h]"
    Write-Host "  -c  clean the build directory"
    Write-Host "  -d  specify the build directory"
    Write-Host "  -h  display help"
    exit
}

function clean_build_directory {
    Write-Host "Cleaning the build directory"
    Remove-Item -Recurse -Force $BUILD_DIR
}

function create_build_directory {
    Write-Host "Creating the build directory"
    New-Item -ItemType Directory -Force -Path $BUILD_DIR
}

# Main logic
if ($h) {
    display_help
}

if (Test-Path $BUILD_DIR) {
    if ($c) {
        clean_build_directory
    } else {
        Write-Host "Build directory already exists"
    }
} else {
    create_build_directory
}

Set-Location -Path $BUILD_DIR
cmake ..
make
Set-Location ..