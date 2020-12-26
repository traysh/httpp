#pragma once

#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>

#include "mock.hpp"

#define MOCK(ORIGINAL, TARGET) \
    extern Mock<decltype(ORIGINAL)> TARGET;

namespace mockable {
MOCK(::select, select);
MOCK(::accept, accept);
MOCK(::listen, listen);
MOCK(::close, close);
extern Mock<int(int, int, int)> fcntl;
}


