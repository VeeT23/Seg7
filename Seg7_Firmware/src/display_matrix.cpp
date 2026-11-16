#include "display_matrix.hpp"

// Reset dimensions and remove all data
void DisplayMatrix::clear_matrix()
{
    rows = 0;
    columns = 0;
    data.clear();
}

// Add rows to the matrix
void DisplayMatrix::add_rows(uint16_t amount)
{
    if (amount == 0)
        return;

    uint16_t new_rows = rows + amount;

    std::vector<CRGB> new_data(
        static_cast<size_t>(columns) * new_rows,
        CRGB::Black
    );

    // Copy existing columns into the expanded matrix
    for (uint16_t column = 0; column < columns; column++)
    {
        for (uint16_t row = 0; row < rows; row++)
        {
            new_data[column * new_rows + row] =
                data[column * rows + row];
        }
    }

    rows = new_rows;
    data = std::move(new_data);
}

// Add columns to the matrix
void DisplayMatrix::add_columns(uint16_t amount)
{
    if (amount == 0)
        return;

    columns += amount;

    // New columns are appended and initialized to black
    data.resize(
        static_cast<size_t>(columns) * rows,
        CRGB::Black
    );
}

// Set every cell to black without changing dimensions
void DisplayMatrix::clear_data()
{
    std::fill(data.begin(), data.end(), CRGB::Black);
}

// Get the color of a cell
CRGB DisplayMatrix::get_cell(int column, int row) const
{
    if (column < 0 || row < 0 ||
        column >= columns || row >= rows)
    {
        return CRGB::Black;
    }

    return data[column * rows + row];
}

// Set the color of a cell
void DisplayMatrix::set_cell(int column, int row, CRGB color)
{
    if (column < 0 || row < 0 ||
        column >= columns || row >= rows)
    {
        return;
    }

    data[column * rows + row] = color;
}

void DisplayMatrix::fill(CRGB color)
{
    for(CRGB& c : data)
    {
        c = color;
    }
}