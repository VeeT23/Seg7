#include "router.hpp"

void Router::begin_serial(unsigned long baudrate)
{
    Serial.begin(baudrate);
}

void Router::begin_bluetooth(String device_name)
{
    bluetooth.begin(device_name);
}

void Router::println(const BroadcastType type, const std::string &message)
{
    const std::string prefix = get_message_prefix(type);
    const std::string formatted_message = prefix + message;

    Serial.println(formatted_message.c_str());
    bluetooth.println(formatted_message.c_str());
}

void Router::print(const BroadcastType type, const std::string &message)
{

    const std::string prefix = get_message_prefix(type);
    const std::string formatted_message = prefix + message;

    Serial.print(message.c_str());
    bluetooth.print(message.c_str());
}

bool Router::has_data()
{
    return Serial.available() > 0 || bluetooth.available() > 0;
}

std::vector<std::string> Router::get_data()
{
    std::vector<std::string> results;

    auto read_lines = [&results](auto &port, std::string &buffer)
    {
        while (port.available() > 0)
        {
            char c = port.read();

            if (c == '\n')
            {
                if (!buffer.empty() && buffer.back() == '\r')
                {
                    buffer.pop_back();
                }

                results.push_back(buffer);
                buffer.clear();
            }
            else
            {
                buffer += c;
            }
        }
    };

    // Read USB Serial
    read_lines(Serial, serial_buffer);

    // Read Bluetooth
    read_lines(bluetooth, bluetooth_buffer);

    return results;
}

bool Router::bluetooth_connected()
{
    return bluetooth.hasClient();
}

std::string Router::get_message_prefix(const BroadcastType type)
{
    switch (type)
    {
    case BroadcastType::STATUS:
        return "STATUS: ";

    case BroadcastType::WARNING:
        return "WARNING: ";

    case BroadcastType::ERROR:
        return "ERROR: ";

    case BroadcastType::PROG_OUT:
        return "PROGRAM_OUT: ";

    default:
        return "";
    }
}