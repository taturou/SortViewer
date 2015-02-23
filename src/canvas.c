#include <pebble.h>
#include "canvas.h"
#include "font.h"

#define FRAME_MARGIN    (8)
#define BMP_ORIGIN_X    (2)
#define BMP_ORIGIN_Y    (10)
#define FONT_W          (8)
#define FONT_H          (10)
#define FONT_SPACE_W    (5)

typedef struct bitmap {
    uint8_t *data;
    uint16_t data_size;
    GSize size;
} Bitmap;
    
typedef struct canvas {
    Layer *layer;
    GRect window_frame;
    Sort *sort;
    Bitmap bitmap;
} Canvas;

static void s_layer_update_callback(Layer *layer, GContext *ctx);
static void s_math_cut_figure2(int num, int figure[2]);
inline static int s_bmp_calc_index(const GSize *size, int x, int y);
inline static uint8_t s_bmp_get(const Bitmap *bmp, int x, int y);
inline static void s_bmp_set(Bitmap *bmp, int x, int y, uint8_t val);
static void s_bmp_draw_font(Bitmap *bmp, GPoint offset, const Font *font);

Canvas *canvas_create(GRect window_frame, Sort *sort) {
    Canvas *canvas = NULL;

    if (sort != NULL) {
        GRect frame = (GRect){
            .origin = {
                window_frame.origin.x + FRAME_MARGIN,
                window_frame.origin.y
            },
            .size = {
                sort_get_data(sort)->num_element * 2,
                window_frame.size.h - FRAME_MARGIN
            }
        };

        Layer *layer = layer_create_with_data(frame, sizeof(Canvas));
        if (layer != NULL) {
            canvas = (Canvas*)layer_get_data(layer);
            canvas->bitmap.size.w = frame.size.w;
            canvas->bitmap.size.h = 7 * FONT_H;
            canvas->bitmap.data_size = canvas->bitmap.size.w * canvas->bitmap.size.h;
            canvas->bitmap.data = calloc(canvas->bitmap.data_size, 1);
            if (canvas->bitmap.data != NULL) {
                canvas->layer = layer;
                canvas->window_frame = frame;
                canvas->sort = sort;
                layer_set_update_proc(layer, s_layer_update_callback);
            } else {
                layer_destroy(layer);
                canvas = NULL;
            }
        }
    }
    return canvas; 
}

void canvas_destroy(Canvas *canvas) {
    if (canvas != NULL) {
        if (canvas->bitmap.data != NULL) {
            free(canvas->bitmap.data);
        }
        layer_destroy(canvas->layer);
    }
}

Layer *canvas_get_layer(const Canvas *canvas) {
    return canvas->layer;
}

void canvas_mark_dirty(Canvas *canvas) {
    layer_mark_dirty(canvas->layer);
}

void canvas_set_time(Canvas *canvas) {
    Bitmap *bmp = &canvas->bitmap;

    memset(bmp->data, 0, bmp->data_size);
    
    // calc time
    time_t tim = time(NULL);
    struct tm *ltim = localtime(&tim);

    int hour[2], min[2];
    s_math_cut_figure2(ltim->tm_hour, hour);
    s_math_cut_figure2(ltim->tm_min, min);

    // draw time
    GPoint origin = {.x = FONT_W, .y = FONT_H};

    // draw time: HH
    origin.x = 0;
    s_bmp_draw_font(bmp, origin, &font_number[hour[1]]);

    origin.x += (font_number[hour[1]].size.w * FONT_W) + FONT_SPACE_W;
    s_bmp_draw_font(bmp, origin, &font_number[hour[0]]);
    
    // draw time: :(colon)
    origin.x += (font_number[hour[0]].size.w * FONT_W) + FONT_SPACE_W;
    s_bmp_draw_font(bmp, origin, &font_colon);
    
    // draw time: MM
    origin.x += (font_colon.size.w * FONT_W) + FONT_SPACE_W;
    s_bmp_draw_font(bmp, origin, &font_number[min[1]]);

    origin.x += (font_number[min[1]].size.w * FONT_W) + FONT_SPACE_W;
    s_bmp_draw_font(bmp, origin, &font_number[min[0]]);
}

static void s_layer_update_callback(Layer *layer, GContext *ctx) {
    Canvas *canvas = (Canvas*)layer_get_data(layer);
    Sort *sort = canvas->sort;
    SortData *data = sort_get_data(sort);
    Bitmap *bmp = &canvas->bitmap;
    
    graphics_context_set_stroke_color(ctx, GColorBlack);

    for (int i = 0; i < data->num_element; i++) {
        // draw elements
#if 0        
        graphics_draw_line(ctx, 
                           (GPoint){
                               i * 2,
                               canvas->window_frame.size.h
                           },
                           (GPoint){
                               i * 2,
                               canvas->window_frame.size.h - data->elements[i]
                           });
#else
        graphics_draw_line(ctx, 
                           (GPoint){
                               i * 2,
                               canvas->window_frame.size.h
                           },
                           (GPoint){
                               i * 2,
                               canvas->window_frame.size.h - data->num_element
                           });
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_draw_pixel(ctx,
                            (GPoint){
                                i * 2,
                                canvas->window_frame.size.h - data->elements[i]
                            });
        graphics_context_set_stroke_color(ctx, GColorBlack);
#endif
        
        // draw clock
        int x = i;
        GPoint p = (GPoint){BMP_ORIGIN_X, BMP_ORIGIN_Y};
        for (int y = 0; y < bmp->size.h; y++) {
            if (s_bmp_get(bmp, ((data->elements[i] - 1) * 2) + 0, y) == 1) {
                graphics_draw_pixel(ctx, (GPoint){p.x + (x * 2) + 0, p.y + y});
            }
#if 0
            if (s_bmp_get(bmp, ((data->elements[i] - 1) * 2) + 1, y) == 1) {
                graphics_draw_pixel(ctx, (GPoint){p.x + (x * 2) + 1, p.y + y});
            }
#endif
        }
    }
    
    // draw algorithm specified graphic
    sort_draw(sort, ctx);
}

static void s_math_cut_figure2(int num, int figure[2]) {
    figure[0] = num % 10;
    figure[1] = (num / 10) % 10;
}

inline static int s_bmp_calc_index(const GSize *size, int x, int y) {
    return x + (size->w * y);
}

inline static uint8_t s_bmp_get(const Bitmap *bmp, int x, int y) {
    return bmp->data[s_bmp_calc_index(&bmp->size, x, y)];
}

inline static void s_bmp_set(Bitmap *bmp, int x, int y, uint8_t val) {
    bmp->data[s_bmp_calc_index(&bmp->size, x, y)] = val;
}

static void s_bmp_draw_font(Bitmap *bmp, GPoint offset, const Font *font) {
    for (int fy = 0; fy < font->size.h; fy++) {
        for (int fx = 0; fx < font->size.w; fx++) {
            if (font->data[fx + (font->size.w * fy)] == 1) {
                for (int by = 0; by < FONT_H; by++) {
                    for (int bx = 0; bx < FONT_W; bx++) {
                        s_bmp_set(bmp, 
                                  offset.x + (fx * FONT_W) + bx,
                                  offset.y + (fy * FONT_H) + by,
                                  1);
                    }
                }
            }
        }
    }
}