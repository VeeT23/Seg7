#pragma once

#include "panel.hpp"

class DotPanel : public Panel
{
public:
    enum class DotState : uint8_t
    {
        NONE = 0,
        BOTTOM = 1,
        TOP = 2,
        BOTH = 3
    };

    void set_dots(DotState value)
    {
        state = value;
    };

    std::vector<CRGB> get_leds(const DisplayMatrix &matrix) override
    {
        std::vector<CRGB> leds(NUMBER_OF_LEDS, CRGB::Black);
        const uint16_t offset = get_offset();

        if (state == DotState::BOTTOM || state == DotState::BOTH)
        {
            for (int i = 0; i < 3; i++)
            {
                leds[i] = matrix.get_cell(offset, i);
            }
        }

        if (state == DotState::TOP || state == DotState::BOTH)
        {
            for (int j = 0; j < 3; j++)
            {
                leds[j + 3] = matrix.get_cell(offset, j + 7);
            }
        }

        return leds;
    }

    uint8_t get_width() const override { return WIDTH; }
    uint8_t get_height() const override { return HEIGHT; }
    uint16_t get_number_of_leds() const override { return NUMBER_OF_LEDS; }
    PanelType get_panel_type() const override { return PanelType::DotPanel; }

private:
    static constexpr uint8_t WIDTH = 1;
    static constexpr uint8_t HEIGHT = 11;
    static constexpr uint8_t NUMBER_OF_LEDS = 6;

    DotState state = DotState::NONE;
};
