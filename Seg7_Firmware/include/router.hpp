
#pragma once

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <string>

class Router
{
private:
    BluetoothSerial bluetooth;

    std::string serial_buffer;
    std::string bluetooth_buffer;

public:
    Router() {}

    void begin_serial(unsigned long baudrate = 115200)
    {
        Serial.begin(baudrate);
    }

    void begin_bluetooth(String device_name)
    {
        bluetooth.begin(device_name);
    }

    void println(const std::string &message)
    {
        Serial.println(message.c_str());
        bluetooth.println(message.c_str());
    }

    void print(const std::string &message)
    {
        Serial.print(message.c_str());
        bluetooth.print(message.c_str());
    }

    bool has_data()
    {
        return Serial.available() > 0 || bluetooth.available() > 0;
    }

    std::string get_data()
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

    bool bluetooth_connected()
    {
        return bluetooth.hasClient();
    }
};
