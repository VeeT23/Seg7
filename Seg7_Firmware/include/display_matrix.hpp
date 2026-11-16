#pragma once

#include <FastLED.h>
#include <vector>

class DisplayMatrix
{
public:

    void clear_matrix();
    void add_rows(uint16_t amount);
    void add_columns(uint16_t amount);

    uint16_t get_width() const { return columns; }
    uint16_t get_height() const { return rows; }
    

    void clear_data();
    CRGB get_cell(int column, int row) const;
    void set_cell(int column, int row, CRGB color);

    void fill(CRGB color);

private:
    uint16_t rows = 0;
    uint16_t columns = 0;
    std::vector<CRGB> data;
};