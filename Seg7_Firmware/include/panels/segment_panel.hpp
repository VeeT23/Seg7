#pragma once

#include "panel.hpp"

class SegmentPanel : public Panel
{
public:
    void set_digit(uint8_t value)
    {
        number = value % 10; // Mod operation to avoid invalid data
    };

    std::vector<CRGB> get_leds(const DisplayMatrix &matrix) override
    {
        std::vector<CRGB> leds(NUMBER_OF_LEDS, CRGB::Black);

        // Segment order: c, b, a, f, e, g, d
        static constexpr uint8_t digit_map[10] = {
            0b01011111, // 0
            0b00000011, // 1
            0b01110110, // 2
            0b01100111, // 3
            0b00101011, // 4
            0b01101101, // 5
            0b01111101, // 6
            0b00000111, // 7
            0b01111111, // 8
            0b01101111  // 9
        };

        // Matrix coordinates relative to offset.
        // LED order: c, b, a, f, e, g, d
        static constexpr uint8_t column_offsets[35] = {
            // C
            0, 0, 0, 0, 0,
            // B
            0, 0, 0, 0, 0,
            // A
            1, 2, 3, 4, 5,
            // F
            6, 6, 6, 6, 6,
            // E
            6, 6, 6, 6, 6,
            // G
            5, 4, 3, 2, 1,
            // D
            1, 2, 3, 4, 5};

        static constexpr uint8_t row_offsets[35] = {
            // C
            0, 1, 2, 3, 4,
            // B
            6, 7, 8, 9, 10,
            // A
            10, 10, 10, 10, 10,
            // F
            10, 9, 8, 7, 6,
            // E
            4, 3, 2, 1, 0,
            // G
            5, 5, 5, 5, 5,
            // D
            0, 0, 0, 0, 0};

        const uint8_t mask = digit_map[number];
        const uint16_t offset = get_offset();

        for (uint8_t led = 0; led < NUMBER_OF_LEDS; led++)
        {
            const uint8_t segment = led / 5;

            if (mask & (1 << segment))
            {
                leds[led] = matrix.get_cell(
                    offset + column_offsets[led],
                    row_offsets[led]);
            }
        }

        return leds;
    }

    uint8_t get_width() const override { return WIDTH; }
    uint8_t get_height() const override { return HEIGHT; }
    uint16_t get_number_of_leds() const override { return NUMBER_OF_LEDS; }
    PanelType get_panel_type() const override { return PanelType::SegmentPanel; }

private:

    static constexpr uint8_t WIDTH = 7;
    static constexpr uint8_t HEIGHT = 11;
    static constexpr uint8_t NUMBER_OF_LEDS = 35;

    uint8_t number = 0;
};
