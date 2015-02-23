#include <pebble.h>
#include "sort_bucket.h"

static char *s_get_name(void);
static uint16_t s_get_ctx_size(void);
static bool s_open(void *ctx, SortData *data);
static void s_close(void *ctx, SortData *data);
static void s_next(void *ctx, SortData *data, bool *is_end);
static void s_draw(void *ctx, SortData *data, GContext *gctx);

SortAlgorithm sort_algorithm_bucket = {
    s_get_name,
    s_get_ctx_size,
    s_open,
    s_close,
    s_next,
    s_draw
};

typedef struct sort_position {
    uint8_t *tmp_elements;
    uint16_t index;
} SortPosition;

static char *s_get_name(void) {
    static char *name = "Bucket";
    return name;
}

static uint16_t s_get_ctx_size(void) {
    return sizeof(SortPosition);
}

static bool s_open(void *ctx, SortData *data) {
    bool ret = false;
    SortPosition *pos = (SortPosition*)ctx;

    pos->tmp_elements = calloc(data->num_element, sizeof(uint8_t));
    if (pos->tmp_elements != NULL) {
        memcpy(pos->tmp_elements, data->elements, data->num_element);
        pos->index = 0;
        ret = true;
    }
    return ret;
}

static void s_close(void *ctx, SortData *data) {
    (void)data;
    SortPosition *pos = (SortPosition*)ctx;
    
    if (pos->tmp_elements != NULL) {
        free(pos->tmp_elements);
        pos->tmp_elements = NULL;
        pos->index = 0;
    }
}

static void s_next(void *ctx, SortData *data, bool *is_end) {
    SortPosition *pos = (SortPosition*)ctx;

    uint16_t val = pos->tmp_elements[pos->index];
    data->elements[val - 1] = val;
    pos->index++;
    if (data->num_element < pos->index) {
        *is_end = true;
    } else {
        *is_end = false;
    }
}

static void s_draw(void *ctx, SortData *data, GContext *gctx) {
    (void)data;
    SortPosition *pos = (SortPosition*)ctx;
    
    graphics_draw_pixel(gctx,
                       (GPoint){(pos->index) * 2, 0});
}
