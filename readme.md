# rcb
:warning: **WORK IN PROGRESS**: This is currently a work in progress and may not have full functionality.

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)


This is a simple command line tool that backs up reddit chat messages sent by the user.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

You need to have the following installed:

- C compiler (GCC, Clang, etc.)
- CMake
- libcurl

### Building
1. Clone the repository:
```bash
git clone https:/github.com/dvidakk/rcb
```
2.  Navigate to the build directory:
```bash
# Create a build directory
mkdir build
# Change to the build directory
cd build
```
3.  Build the project using CMake:
```bash
cmake ..
make -j$(nproc)
```

## Usage

Run the application with the following command:
```bash
./rcb
```
## Dependencies
- cJSON
- libcurl

## License

This project is licensed under the GPLv3 License - see the [LICENSE.md](LICENSE.md) file for details
