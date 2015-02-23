#pragma once
#include <pebble.h>
#include <sort.h>

typedef struct canvas Canvas;

Canvas *canvas_create(GRect window_frame, Sort *sort);
void canvas_destroy(Canvas *canvas);
Layer *canvas_get_layer(const Canvas *canvas);
void canvas_mark_dirty(Canvas *canvas);
void canvas_set_time(Canvas *canvas);