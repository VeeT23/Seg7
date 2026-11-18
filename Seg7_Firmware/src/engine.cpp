#include "engine.hpp"

#include <iterator>

#include <sstream>
#include <string>
#include <iostream>

void Engine::set_clock(RTC_DS3231 &clock)
{
    rtc = clock;
}

void Engine::pump_input(std::vector<std::string> data)
{
    inbound_buffer.insert(
        inbound_buffer.end(),
        std::make_move_iterator(data.begin()),
        std::make_move_iterator(data.end()));
}

void Engine::update()
{
    for (const std::string line : inbound_buffer)
    {
        handle_data(line);
    }

    inbound_buffer.clear();
}

void Engine::handle_data(const std::string &data)
{
    std::istringstream stream(data);

    std::string action;
    std::string command;

    // Extract the first two words
    stream >> action >> command;

    // Everything remaining is the arguments
    std::string args;
    std::getline(stream >> std::ws, args);

    if (action == "SET")
    {
        handle_set(command, args);
    }
    else if (action == "GET")
    {
        handle_get(command, args);
    }
    else
    {
        post_message(Router::BroadcastType::ERROR, "Unknown action: " + action);
    }
}

void Engine::handle_set(const std::string &command, const std::string &args)
{
    if (command == "TIME")
    {
    }
    else
    {
        post_message(Router::BroadcastType::ERROR, "Unknown set command: " + command);
    }
}

void Engine::handle_get(const std::string &command, const std::string &args)
{
    if (command == "TIME")
    {
        post_message(Router::BroadcastType::STATUS, "TIME: " + rtc.now().timestamp());
    }
    else
    {
        post_message(Router::BroadcastType::ERROR, "Unknown get command: " + command);
    }
}

void Engine::drain_output(Router &router)
{
    for (Message &msg : outbound_buffer)
    {
        router.println(msg.first, msg.second);
    }
    outbound_buffer.clear();
}

void Engine::post_message(Router::BroadcastType type, std::string message)
{
    outbound_buffer.push_back({type, message});
}