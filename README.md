[![Coverage Status](https://coveralls.io/repos/github/traysh/httpp/badge.svg?branch=master)](https://coveralls.io/github/traysh/httpp?branch=master)

In-development easy to use Modern C++ web backend framework.

### Table of Contents
- [Usage Example](#usage-example)
- [Goals](#goals)
- [How to build on Mac or Linux](#how-to-build-on-mac-or-linux)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
- [Roadmap](#roadmap)
- [TODO](#todo)


# Usage Example

Minimal server example:

```c++
#include "server.hpp"

int main() {
    const unsigned short port = 9933;

    Server server;

    auto& router = server.GetRouter();
    using Method = HTTP::MethodType;
    router.Add({
        { "/hi", Method::Get,
            [](auto& response) {
                response << "Hello from " << request.Path;
        }},
        { "/echo", Method::Post,
            [](const auto& request, auto& response) {
                response << request.Body.CStr();
        }},
    });
    router.SetNotFoundHandler([](auto& response) {
        response.Status = HTTPResponseStatus::Type::NotFound;
        response << "sorry, I don't know that URL";
    });

    server.Serve(port);

    return 0;
}
```

Output from calling these endpoints:

```shell
$ curl localhost:9933/hi  
Hello from /hi%
$ curl --data "hello world" localhost:9933/echo
hello world%
$ curl -v localhost:9933/blah
Trying ::1...
* TCP_NODELAY set
* Connection failed
* connect to ::1 port 9933 failed: Connection refused
*   Trying 127.0.0.1...
* TCP_NODELAY set
* Connected to localhost (127.0.0.1) port 9933 (#0)
> GET /blah HTTP/1.1
> Host: localhost:9933
> User-Agent: curl/7.54.0
> Accept: */*
>
< HTTP/1.1 404 Not Found
< CONNECTION: Close
<
* Connection #0 to host localhost left intact
sorry, I don't know that URL%
```

# Goals

- **Easy to use** as most high-level language frameworks, such as the ones available for Node.js and Python.<br/>
- **Easy to use** the right way
- **Cross platform** must support at least Linux and MacOS
- **Cross architecture** must support at least x86 and ARMv5 and later
- **TDD** to ensure testable and high quality code
- **Good performance** though good usability is preferred over it
- **Secure**


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

First, install a compiler: *Xcode* or *Command Line Tools*

Then, install homebrew package manager: https://brew.sh/#install

Then use it to install the other requirements: `brew install cmake gcovr`

You can also use homebrew's LLVM compiler instead, but will need to adjust
CC and CXX environment variables to point to clang and clang++ binaries
installed by homebrew.

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

Note: this is just a proposal to be discussed between project members

- version 1.0
   - [x] HTTP/1 support
   - [x] multithreaded requests processor
   - [x] request parser providing access to HTTP request method, headers and body
   - [ ] JSON parser for this content-type
   - [x] router for endpoints
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
