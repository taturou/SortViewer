#include <pebble.h>
#include "canvas.h"

typedef struct canvas {
    Layer *layer;
    GRect window_frame;
    Sort *sort;
} Canvas;

static void s_layer_update_callback(Layer *layer, GContext *ctx);

Canvas *canvas_create(GRect window_frame, Sort *sort) {
    Canvas *canvas = NULL;

    if (sort != NULL) {
        GRect frame = (GRect){.origin = {0, 20}, .size = {window_frame.size.w, window_frame.size.h - 20}};
        Layer *layer = layer_create_with_data(frame, sizeof(Canvas));
        if (layer != NULL) {
            canvas = (Canvas*)layer_get_data(layer);
            canvas->layer = layer;
            canvas->window_frame = frame;
            canvas->sort = sort;
            layer_set_update_proc(layer, s_layer_update_callback);
        }
    }
    return canvas; 
}

void canvas_destroy(Canvas *canvas) {
    if (canvas != NULL) {
        layer_destroy(canvas->layer);
    }
}

Layer *canvas_get_layer(const Canvas *canvas) {
    return canvas->layer;
}

void canvas_mark_dirty(Canvas *canvas) {
    layer_mark_dirty(canvas->layer);
}

static void s_layer_update_callback(Layer *layer, GContext *ctx) {
    Canvas *canvas = (Canvas*)layer_get_data(layer);
    Sort *sort = canvas->sort;
    SortData *data = sort_get_data(sort);
    
    graphics_context_set_stroke_color(ctx, GColorBlack);

    GPoint p0, p1;
    for (int i = 0; i < data->num_element; i++) {
        p0.x = i * 2;
        p0.y = canvas->window_frame.size.h;

        p1.x = p0.x;
        p1.y = p0.y - (data->elements[i] * 2);

        graphics_draw_line(ctx, p0, p1);
    }

    sort_draw(sort, ctx);
}