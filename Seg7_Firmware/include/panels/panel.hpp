#pragma once

#include <vector>
#include <stdint.h>
#include <FastLED.h>
#include "display_matrix.hpp"

class Panel
{
public:

    enum class PanelType : uint8_t
    {
        SegmentPanel = 0,
        DotPanel = 1,
        Unknown = 255
    };

    uint16_t get_offset() { return offset; }
    void set_offset(uint16_t value) { offset = value; }

    virtual std::vector<CRGB> get_leds(const DisplayMatrix &) = 0;

    virtual uint8_t get_width() const { return 0; };
    virtual uint8_t get_height() const { return 0; };
    virtual uint16_t get_number_of_leds() const { return 0; };
    virtual PanelType get_panel_type() const { return PanelType::Unknown; }

private:
    uint16_t offset = 0;
};