#pragma once
#include <pebble.h>
    
typedef struct font {
    GSize size;
    const uint8_t *data;
} Font;

const Font font_number[10];
const Font font_colon;