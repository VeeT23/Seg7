#include "engine.hpp"

#include <iterator>

void Engine::pump_input(std::vector<std::string> data)
{
    inbound_buffer.insert(
        inbound_buffer.end(),
        std::make_move_iterator(data.begin()),
        std::make_move_iterator(data.end())
    );
}

void Engine::update()
{

    inbound_buffer.clear();
}


void Engine::drain_output(Router& router)
{
    for(Message& msg : outbound_buffer)
    {
        router.println(msg.first, msg.second);
    }
    outbound_buffer.clear();
}

void Engine::post_message(Router::BroadcastType type, std::string message)
{
    outbound_buffer.push_back({type,message});
}