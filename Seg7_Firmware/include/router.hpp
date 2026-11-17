
#pragma once

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <string>

class Router
{
public:
    enum class BroadcastType : uint8_t
    {
        STATUS = 0,
        WARNING = 1,
        ERROR = 2,
        PROG_OUT = 3
    };

    Router() {}

    void begin_serial(unsigned long baudrate = 115200);
    void begin_bluetooth(String device_name);

    void println(const BroadcastType type, const std::string &message);
    void print(const BroadcastType type, const std::string &message);

    bool has_data();
    std::vector<std::string> get_data();
    
    bool bluetooth_connected();

private:
    BluetoothSerial bluetooth;

    std::string serial_buffer;
    std::string bluetooth_buffer;

    std::string get_message_prefix(const BroadcastType type);

};
