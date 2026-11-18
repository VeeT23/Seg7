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

    if (action == "PING")
    {
        post_message(Router::BroadcastType::NONE, "PONG");
    }
    else if (action == "SET")
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
        int year, month, day, hour, minute, second;
        char extra;

        // Validate argument format and reject trailing characters.
        if (sscanf(args.c_str(), "%d-%d-%d %d:%d:%d %c",
                   &year, &month, &day,
                   &hour, &minute, &second, &extra) != 6)
        {
            post_message(Router::BroadcastType::ERROR,
                         "Invalid time format. Expected: YYYY-MM-DD HH:MM:SS");
            return;
        }

        // Validate date and time ranges.
        if (year < 2000 || year > 2099 ||
            month < 1 || month > 12 ||
            day < 1 || day > 31 ||
            hour < 0 || hour > 23 ||
            minute < 0 || minute > 59 ||
            second < 0 || second > 59)
        {
            post_message(Router::BroadcastType::ERROR,
                         "Invalid date or time values");
            return;
        }

        // Validate the actual calendar date.
        DateTime dt(year, month, day, hour, minute, second);

        if (!dt.isValid())
        {
            post_message(Router::BroadcastType::ERROR,
                         "Invalid calendar date");
            return;
        }

        // Apply RTC update.
        rtc.adjust(dt);

        // Report success.
        char buffer[32];
        snprintf(buffer, sizeof(buffer),
                 "%04d-%02d-%02d %02d:%02d:%02d",
                 year, month, day, hour, minute, second);

        post_message(Router::BroadcastType::STATUS,
                     "Set time to: " + std::string(buffer));
    }
    else
    {
        post_message(Router::BroadcastType::ERROR,
                     "Unknown set command: " + command);
    }
}

void Engine::handle_get(const std::string &command, const std::string &args)
{
    if (command == "TIME")
    {
        DateTime now = rtc.now();

        char dateTimeStr[20];
        snprintf(dateTimeStr, sizeof(dateTimeStr),
                 "%04d-%02d-%02d %02d:%02d:%02d",
                 now.year(),
                 now.month(),
                 now.day(),
                 now.hour(),
                 now.minute(),
                 now.second());

        post_message(Router::BroadcastType::STATUS,
                     std::string("TIME: ") + dateTimeStr);
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