#include "display.hpp"

#include <stdexcept>
#include <memory>
#include <Arduino.h>

Display::Display(uint16_t max_leds)
    : max_leds(max_leds),
      connected_leds(0),
      initialized(false),
      leds(new CRGB[max_leds])
{
    matrix.add_rows(11);
}

Display::~Display()
{
    delete[] leds;
}

void Display::show()
{
    if (!initialized)
    {
        return;
    }

    uint16_t led_index = 0;

    // Start from a clean frame so disconnected slots stay dark.
    for (uint16_t i = 0; i < max_leds; i++)
    {
        leds[i] = CRGB::Black;
    }

    for (const auto& panel : panels)
    {
        const std::vector<CRGB> panel_leds = panel->get_leds(matrix);

        for (const CRGB& color : panel_leds)
        {
            if (led_index >= max_leds)
            {
                break;
            }

            leds[led_index++] = color;
        }

        if (led_index >= max_leds)
        {
            break;
        }
    }

    FastLED.show();
}

PanelID Display::add_panel(Panel::PanelType type)
{
    switch (type)
    {
        case Panel::PanelType::SegmentPanel:
            panels.push_back(std::make_unique<SegmentPanel>());
            break;

        case Panel::PanelType::DotPanel:
            panels.push_back(std::make_unique<DotPanel>());
            break;

        default:
            throw std::runtime_error("Unknown panel type");
    }

    Panel& panel = *panels.back();

    panel.set_offset(matrix.get_width());
    matrix.add_columns(panel.get_width());

    connected_leds += panel.get_number_of_leds();

    return static_cast<PanelID>(panels.size() - 1);
}
