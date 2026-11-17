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

std::string Router::get_data()
{
    // Prefer USB Serial if it has a complete line
    while (Serial.available() > 0)
    {
        char c = Serial.read();

        if (c == '\n')
        {
            std::string result = serial_buffer;
            serial_buffer.clear();

            // Remove trailing carriage return (\r)
            if (!result.empty() && result.back() == '\r')
            {
                result.pop_back();
            }

            return result;
        }

        serial_buffer += c;
    }

    // Check Bluetooth
    while (bluetooth.available() > 0)
    {
        char c = bluetooth.read();

        if (c == '\n')
        {
            std::string result = bluetooth_buffer;
            bluetooth_buffer.clear();

            if (!result.empty() && result.back() == '\r')
            {
                result.pop_back();
            }

            return result;
        }

        bluetooth_buffer += c;
    }

    return "";
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