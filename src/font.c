#include <pebble.h>
#include "font.h"

const uint8_t font_number_data[10][3 * 5] = {
    // 0
    {
        1,1,1,
        1,0,1,
        1,0,1,
        1,0,1,
        1,1,1,
    },
    // 1
    {
        1,1,0,
        0,1,0,
        0,1,0,
        0,1,0,
        1,1,1,
    },
    // 2
    {
        1,1,1,
        0,0,1,
        1,1,1,
        1,0,0,
        1,1,1,
    },
    // 3
    {
        1,1,1,
        0,0,1,
        1,1,1,
        0,0,1,
        1,1,1,
    },
    // 4
    {
        1,0,1,
        1,0,1,
        1,1,1,
        0,0,1,
        0,0,1,
    },
    // 5
    {
        1,1,1,
        1,0,0,
        1,1,1,
        0,0,1,
        1,1,1,
    },
    // 6
    {
        1,1,1,
        1,0,0,
        1,1,1,
        1,0,1,
        1,1,1,
    },
    // 7
    {
        1,1,1,
        0,0,1,
        0,1,0,
        0,1,0,
        0,1,0,
    },
    // 8
    {
        1,1,1,
        1,0,1,
        1,1,1,
        1,0,1,
        1,1,1,
    },
    // 9
    {
        1,1,1,
        1,0,1,
        1,1,1,
        0,0,1,
        1,1,1,
    }
};

const Font font_number[10] = {
    {.size = {3, 5}, .data = font_number_data[0]},
    {.size = {3, 5}, .data = font_number_data[1]},
    {.size = {3, 5}, .data = font_number_data[2]},
    {.size = {3, 5}, .data = font_number_data[3]},
    {.size = {3, 5}, .data = font_number_data[4]},
    {.size = {3, 5}, .data = font_number_data[5]},
    {.size = {3, 5}, .data = font_number_data[6]},
    {.size = {3, 5}, .data = font_number_data[7]},
    {.size = {3, 5}, .data = font_number_data[8]},
    {.size = {3, 5}, .data = font_number_data[9]}
};

const uint8_t font_colon_data[1 * 5] = {
    0,
    1,
    0,
    1,
    0,
};

const Font font_colon = {
    .size = {1, 5}, .data = font_colon_data
};