How to build on Mac

1. checkout submodules: `git submodule update --init`
2. create build the dir and change to it: `mkdir -p build && cd build`
3. run cmake to generate the Makefile: `cmake ..`
4. build the project:
    - build using a single job: `make`
    - optionally, build using multiple cores instead: `make -j9`
5. build and run tests: `make test` or `make -j9 test`
