#include <pebble.h>
#include "sort_merge.h"

static char *s_get_name(void);
static uint16_t s_get_ctx_size(void);
static bool s_open(void *ctx, SortData *data);
static void s_close(void *ctx, SortData *data);
static void s_next(void *ctx, SortData *data, bool *is_end);
static void s_draw(void *ctx, SortData *data, GContext *gctx);

SortAlgorithm sort_algorithm_merge = {
    s_get_name,
    s_get_ctx_size,
    s_open,
    s_close,
    s_next,
    s_draw
};

typedef struct sort_position {
    uint8_t *tmp_elements;
    uint16_t div_size;
    uint16_t max_div_size;
    uint16_t div_index;
    uint16_t max_div_index;
    uint16_t l_index;
    uint16_t r_index;
    uint16_t max_l_index;
    uint16_t max_r_index;
    uint16_t insert_index;
} SortPosition;

static char *s_get_name(void) {
    static char *name = "Merge";
    return name;
}

static uint16_t s_get_ctx_size(void) {
    return sizeof(SortPosition);
}

static bool s_open(void *ctx, SortData *data) {
    bool ret = false;
    SortPosition *pos = (SortPosition*)ctx;

    pos->tmp_elements = calloc(data->num_element, 1);
    if (pos->tmp_elements != NULL) {
        memcpy(pos->tmp_elements, data->elements, data->num_element);
    
        pos->div_size = 1;
        pos->max_div_size = data->num_element / 2;
        pos->div_index = 0;
        pos->max_div_index = data->num_element / pos->div_size;
        pos->l_index = pos->div_index * pos->div_size;
        pos->r_index = pos->l_index + pos->div_size;
        pos->max_l_index = pos->l_index + pos->div_size - 1;
        pos->max_r_index = pos->r_index + pos->div_size - 1;
        pos->insert_index = pos->l_index;
        ret = true;
    }
/*
    APP_LOG(APP_LOG_LEVEL_DEBUG, " DivSz, MaxDivSz, DivIdx, MaxDivIdx, LIdx, RIdx, MaxLIdx, MaxRIdx, InsIdx");
    APP_LOG(APP_LOG_LEVEL_DEBUG,
            " %5d, %8d, %6d, %9d, %4d, %4d, %7d, %7d, %6d",
            pos->div_size,
            pos->max_div_size,
            pos->div_index,
            pos->max_div_index,
            pos->l_index,
            pos->r_index,
            pos->max_l_index,
            pos->max_r_index,
            pos->insert_index);
*/
    return ret;
}

static void s_close(void *ctx, SortData *data) {
    (void)data;    
    SortPosition *pos = (SortPosition*)ctx;
    
    free(pos->tmp_elements);
    pos->tmp_elements = NULL;
}

static void s_next(void *ctx, SortData *data, bool *is_end) {
    SortPosition *pos = (SortPosition*)ctx;
    *is_end = false;
    
    if (pos->max_l_index < pos->l_index) {
        pos->tmp_elements[pos->insert_index] = data->elements[pos->r_index];
        pos->insert_index++;
        pos->r_index++;
    } else if (pos->max_r_index < pos->r_index) {
        pos->tmp_elements[pos->insert_index] = data->elements[pos->l_index];
        pos->insert_index++;
        pos->l_index++;
    } else {
        if (data->elements[pos->l_index] == data->elements[pos->r_index]) {
            pos->tmp_elements[pos->insert_index] = data->elements[pos->l_index];
            pos->insert_index++;
            pos->l_index++;
            pos->tmp_elements[pos->insert_index] = data->elements[pos->r_index];
            pos->insert_index++;
            pos->r_index++;
        } else if (data->elements[pos->l_index] < data->elements[pos->r_index]) {
            pos->tmp_elements[pos->insert_index] = data->elements[pos->l_index];
            pos->insert_index++;
            pos->l_index++;
        } else if (data->elements[pos->l_index] > data->elements[pos->r_index]) {
            pos->tmp_elements[pos->insert_index] = data->elements[pos->r_index];
            pos->insert_index++;
            pos->r_index++;
        }
    }

    if ((pos->max_l_index < pos->l_index)
        && (pos->max_r_index < pos->r_index)) {
        memcpy(data->elements, pos->tmp_elements, data->num_element);

        pos->div_index += 2;
        if (pos->max_div_index > pos->div_index) {
            pos->l_index = pos->div_index * pos->div_size;
            pos->r_index = pos->l_index + pos->div_size;
            pos->max_l_index = pos->l_index + pos->div_size - 1;
            pos->max_r_index = pos->r_index + pos->div_size - 1;
            pos->insert_index = pos->l_index;
        } else {
            pos->div_size *= 2;
            pos->div_index = 0;
            pos->max_div_index = data->num_element / pos->div_size;
            pos->l_index = pos->div_index * pos->div_size;
            pos->r_index = pos->l_index + pos->div_size;
            pos->max_l_index = pos->l_index + pos->div_size - 1;
            pos->max_r_index = pos->r_index + pos->div_size - 1;
            pos->insert_index = pos->l_index;
            if (pos->max_div_size < pos->div_size) {
                *is_end = true;
            }
        }
    }
/*
    APP_LOG(APP_LOG_LEVEL_DEBUG,
            "%5d, %8d, %6d, %9d, %4d, %4d, %7d, %7d, %6d",
            pos->div_size,
            pos->max_div_size,
            pos->div_index,
            pos->max_div_index,
            pos->l_index,
            pos->r_index,
            pos->max_l_index,
            pos->max_r_index,
            pos->insert_index);
*/
}

static void s_draw(void *ctx, SortData *data, GContext *gctx) {
    SortPosition *pos = (SortPosition*)ctx;
    
    graphics_draw_line(gctx,
                       (GPoint){(pos->div_index * pos->div_size) * 2, 0},
                       (GPoint){(pos->div_index * pos->div_size + pos->div_size * 2) * 2, 0});
    graphics_draw_pixel(gctx,
                       (GPoint){(pos->l_index) * 2, 2});
    graphics_draw_pixel(gctx,
                       (GPoint){(pos->r_index) * 2, 2});
    graphics_draw_pixel(gctx,
                       (GPoint){(pos->insert_index) * 2, 4});
}