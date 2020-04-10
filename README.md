[![Pull requests](https://img.shields.io/github/issues-pr-raw/xyyeh/cpp_tutorials.svg)](https://github.com/xyyeh/cpp_tutorials/pulls)
[![Opened issues](https://img.shields.io/github/issues-raw/xyyeh/cpp_tutorials.svg)](https://github.com/xyyeh/cpp_tutorials/issues)
[![Documentation](https://img.shields.io/badge/Documentation-latest-blue.svg)](https://github.com/xyyeh/cpp_tutorials/)
[![License](https://img.shields.io/github/license/xyyeh/cpp_tutorials)](https://github.com/xyyeh/cpp_tutorials/LICENSE.md)

# Cpp Tutorial
The repository contains a typical cpp examples, tools and methods that are useful for general real-time programming.

## Build instructions
The repository contains several apps which are prepended with 'app_'. The purpose of each app is detailed in the README.md files in the respective folders. Each of the app can be built by:

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_THIRDPARTY=ON
