# What is it?

In-development C++ web backend framework

# Goals

- easy to use as most high-level language frameworks, such as the ones available for Node.js and Golang
- easy to use the right way
- secure
- good performance
- TDD: good code quality

# How to build on Mac or Linux
## Prerequisites

- llvm
- cmake
- gcovr (for test coverage reports)

## Install prerequisites on Linux

Install prerequisites on Arch Linux: `pacman -Sy llvm cmake gcovr`

Note: the default compiler of most linux distributions is GCC, which is not currently working for building this project. So LLVM is required for now. Thus, it will be required to export it as the C and C++ compiler prior to running cmake:
```shell
export CC=clang
export CXX=clang++
```

## Install prerequisites on Mac

First, install homebrew package manager: https://brew.sh/#install

Then use it to install the other requirements: `brew install llvm cmake gcovr`

You can also use Xcode LLVM compiler instead. But CMake and gcovr aren't installed by Xcode.

## Building

0. check prerequisites for your platform above
1. clone this repo and cwd to your local copy
2. clone submodules:
    ```shell
    git submodule update --init
    ```
3. create the build dir and change to it:
    ```shell
    mkdir -p build && cd build
    ```
4. run cmake to generate the Makefile:
    ```shell
    cmake ..
    ```
5. build the project:
    - build using a single job:  `make`
    - optionally, build using multiple cores instead: `make -j9`
6. build and run tests:
    - build using a single job: `make test`
    - optionally, build using multiple cores instead: `make -j9 test`

# Roadmap

Note this is just a proposal to be discussed between project members

- version 1.0
   - [ ] HTTP/1 support
   - [x] multithreaded requests processor
   - [x] request parser providing access to HTTP request method, headers and body
   - [ ] JSON parser for this content-type
   - [ ] router for endpoints
   - [ ] middleware support
- version 1.5
   - [ ] Use epoll for Linux and kqueue for MacOS (performance improvements)
- version 2.0
   - [ ] websocket support
- version 3.0
   - [ ] HTTP/2.0 support
- version 4.0
   - [ ] QUIC support

# TODO

    Better name for the project
