#include <pebble.h>
#include "sort_babble.h"

static uint16_t s_get_ctx_size(void);
static bool s_open(void *ctx, SortData *data);
static void s_close(void *ctx, SortData *data);
static void s_next(void *ctx, SortData *data, bool *is_end);
static void s_draw(void *ctx, SortData *data, GContext *gctx);

SortAlgorithm sort_algorithm_babble = {
    s_get_ctx_size,
    s_open,
    s_close,
    s_next,
    s_draw
};

typedef struct sort_position {
    uint16_t in;
    uint16_t out;
} SortPosition;

static uint16_t s_get_ctx_size(void) {
    return sizeof(SortPosition);
}

static bool s_open(void *ctx, SortData *data) {
    SortPosition *pos = (SortPosition*)ctx;
    
    pos->in = 1;
    pos->out = data->num_element;
    return true;
}

static void s_close(void *ctx, SortData *data) {
    (void)ctx;
    (void)data;    
}

static void s_next(void *ctx, SortData *data, bool *is_end) {
    SortPosition *pos = (SortPosition*)ctx;

    if (data->elements[pos->in] < data->elements[pos->in - 1]) {
        uint8_t val = data->elements[pos->in];
        data->elements[pos->in] = data->elements[pos->in - 1];
        data->elements[pos->in - 1] = val;
    }
    pos->in++;
    if (pos->out <= pos->in) {
        pos->in = 1;
        pos->out--;
    }
    if (pos->out == 1) {
        *is_end = true;
    } else {
        *is_end = false;
    }
}

static void s_draw(void *ctx, SortData *data, GContext *gctx) {
    SortPosition *pos = (SortPosition*)ctx;
    GPoint p0, p1;
    
    p0.x = 0;
    p0.y = 0;
    p1.x = pos->out * 2;
    p1.y = 0;
    graphics_draw_line(gctx, p0, p1);

    p0.x = 0;
    p0.y = 2;
    p1.x = pos->in * 2;
    p1.y = 2;
    graphics_draw_line(gctx, p0, p1);
}