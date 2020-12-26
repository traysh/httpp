#include <gtest/gtest.h>

#include "mock/listensocket_mocks.hpp"

#include <iostream>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Mocks closing descriptors (sockets)
    mockable::close.Register(
        [](int){
            return 0;
        }
    );


    return RUN_ALL_TESTS();
}
