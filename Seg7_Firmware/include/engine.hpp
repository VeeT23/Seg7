#pragma once

#include <vector>
#include <string>

#include "router.hpp"

// Handles serial commands and interprets dsl bytecode

// CMD: LOAD_PROGRAM 1
// CMD: LOAD_PROGRAM "Rainbow Strobe"

// CMD: SET_SPEED 1.0
// CMD: SET_BRIGHTNESS 1.0
// CMD: SET_SPEED 1.0

class Engine
{
public:
    void pump_input(std::vector<std::string> data);
    void update();
    void drain_output(Router& router);

private:

    void post_message(Router::BroadcastType type, std::string message);

    using Message = std::pair<Router::BroadcastType, std::string>;

    std::vector<std::string> inbound_buffer;
    std::vector<Message> outbound_buffer;
};