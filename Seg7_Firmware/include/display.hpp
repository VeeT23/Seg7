#pragma once

#include <vector>
#include <memory>
#include <FastLED.h>
#include "display_matrix.hpp"
#include "panels/panel.hpp"
#include "panels/segment_panel.hpp"
#include "panels/dot_panel.hpp"

using PanelID = uint8_t;

class Display
{
public:
    explicit Display(uint16_t max_leds);
    ~Display();

    // FastLED requires the data pin at compile time, so it is a template
    // argument: display.begin<LED_PIN>();
    template <uint8_t DataPin>
    void begin()
    {
        if (initialized)
        {
            return;
        }

        FastLED.addLeds<WS2812, DataPin, GRB>(leds, max_leds);
        FastLED.setBrightness(96);
        FastLED.clear(true);
        initialized = true;
    }

    PanelID add_panel(Panel::PanelType type);

    void show();

    DisplayMatrix &get_matrix()  { return matrix; }
    Panel &get_panel(PanelID id) { return *panels.at(id); }

private:
    uint16_t max_leds;
    uint16_t connected_leds;
    bool initialized;

    CRGB *leds;
    DisplayMatrix matrix;

    std::vector<std::unique_ptr<Panel>> panels;
};