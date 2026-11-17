#pragma once

#include <vector>
#include <string>

// Handles serial commands and interprets dsl bytecode

// CMD: LOAD_PROGRAM 1
// CMD: LOAD_PROGRAM "Rainbow Strobe"

// CMD: SET_SPEED 1.0
// CMD: SET_BRIGHTNESS 1.0
// CMD: SET_SPEED 1.0


class Engine {
public:
    std::vector<std::string> handle_incoming_data(std::string data);

private:
    
};