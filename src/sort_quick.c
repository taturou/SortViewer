#include <pebble.h>
#include "sort_quick.h"

static char *s_get_name(void);
static uint16_t s_get_ctx_size(void);
static bool s_open(void *ctx, SortData *data);
static void s_close(void *ctx, SortData *data);
static void s_next(void *ctx, SortData *data, bool *is_end);
static void s_draw(void *ctx, SortData *data, GContext *gctx);

SortAlgorithm sort_algorithm_quick = {
    s_get_name,
    s_get_ctx_size,
    s_open,
    s_close,
    s_next,
    s_draw
};

typedef struct area {
    bool alive;
    uint16_t start_index;
    uint16_t end_index;
    uint16_t pivot;
} Area;

typedef struct sort_position {
    Area *areas;
    int num_area;
    int area_index;
    uint16_t l_index;
    uint16_t r_index;
} SortPosition;

static int s_area_set(SortPosition *pos, const SortData *data, uint16_t start_index, uint16_t end_index);
static int s_area_get_alive(SortPosition *pos, int now_area_index);

static char *s_get_name(void) {
    static char *name = "Quick";
    return name;
}

static uint16_t s_get_ctx_size(void) {
    return sizeof(SortPosition);
}

static bool s_open(void *ctx, SortData *data) {
    bool ret = false;
    SortPosition *pos = (SortPosition*)ctx;
    
    pos->num_area = data->num_element;
    pos->areas = calloc(sizeof(Area), pos->num_area);
    if (pos->areas != NULL) {
        for (int i = 0; i < pos->num_area; i++) {
            pos->areas[i].alive = false;
        }
        pos->area_index = s_area_set(pos, data, 0, data->num_element - 1);
        pos->l_index = pos->areas[pos->area_index].start_index;
        pos->r_index = pos->areas[pos->area_index].end_index;
        ret = true;
    }    
    return ret;
}

static void s_close(void *ctx, SortData *data) {
    (void)data;    
    SortPosition *pos = (SortPosition*)ctx;
    
    if (pos->areas != NULL) {
        free(pos->areas);
        pos->areas = NULL;
    }
}

static void s_next(void *ctx, SortData *data, bool *is_end) {
    SortPosition *pos = (SortPosition*)ctx;
    Area *area = &pos->areas[pos->area_index];
    *is_end = false;

//     APP_LOG(APP_LOG_LEVEL_DEBUG, "== l:%d, r:%d", pos->l_index, pos->r_index);
    for (; pos->l_index <= pos->r_index; pos->l_index++) {
        if (area->pivot <= data->elements[pos->l_index]) {
            break;
        }
    }
    for (; pos->r_index >= pos->l_index; pos->r_index--) {
        if (area->pivot >= data->elements[pos->r_index]) {
            break;
        }
    }
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "=> l:%d, r:%d", pos->l_index, pos->r_index);
    
    // swap
    if (pos->l_index < pos->r_index) {
        uint16_t tmp = data->elements[pos->l_index];
        data->elements[pos->l_index] = data->elements[pos->r_index];
        data->elements[pos->r_index] = tmp;
        pos->l_index++;
        pos->r_index--;
    // 
    } else {
        pos->area_index = -1;
        if ((area->start_index + 1) == area->end_index) {
            pos->area_index = s_area_get_alive(pos, pos->area_index);
        } else {
            if (area->start_index >= (pos->l_index - 1)) {
                if (pos->area_index == -1) {
                    pos->area_index = s_area_get_alive(pos, pos->area_index);
                }            
            } else {
                int area_index = s_area_set(pos, data, area->start_index, pos->l_index - 1);
                if (pos->area_index == -1) {
                    pos->area_index = area_index;
                }
            }
        
            if (pos->l_index >= area->end_index) {
                if (pos->area_index == -1) {
                    pos->area_index = s_area_get_alive(pos, pos->area_index);
                }            
            } else {
                int area_index = s_area_set(pos, data, pos->l_index, area->end_index);
                if (pos->area_index == -1) {
                    pos->area_index = area_index;
                }
            }
        }

//         APP_LOG(APP_LOG_LEVEL_DEBUG, "s_next: pos->area_index:%d", pos->area_index);
        if (pos->area_index == -1) {
             *is_end = true;
        } else {
            pos->l_index = pos->areas[pos->area_index].start_index;
            pos->r_index = pos->areas[pos->area_index].end_index;
        }
        
        area->alive = false;
    }
}

static void s_draw(void *ctx, SortData *data, GContext *gctx) {
    SortPosition *pos = (SortPosition*)ctx;
    Area *area = &pos->areas[pos->area_index];

    graphics_draw_line(gctx,
                       (GPoint){(area->start_index) * 2, 0},
                       (GPoint){(area->end_index) * 2, 0});

    graphics_draw_line(gctx,
                       (GPoint){(area->pivot) * 2, 2},
                       (GPoint){(area->pivot) * 2, 2});

    graphics_draw_line(gctx,
                       (GPoint){(pos->l_index) * 2, 4},
                       (GPoint){(pos->l_index) * 2, 4});

    graphics_draw_line(gctx,
                       (GPoint){(pos->r_index) * 2, 4},
                       (GPoint){(pos->r_index) * 2, 4});
}

// return index of the pos->areas that is set to alive
static int s_area_set(SortPosition *pos, const SortData *data, uint16_t start_index, uint16_t end_index) {
    int area_index = 0;
    Area *area = NULL;
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "s_area_set(,, start:%d, end:%d)", start_index, end_index);
    
    // search not-alived area
    for (area_index = 0; area_index < pos->num_area; area_index++) {
        if (pos->areas[area_index].alive == false) {
            area = &pos->areas[area_index];
            break;
        }
    }

    // set
    area->alive = true;
    area->start_index = start_index;
    area->end_index = end_index;
    
    // calc pivot
    uint16_t min, max;
    min = max = data->elements[start_index];
    for (uint16_t i = start_index; i <= end_index; i++) {
        if (data->elements[i] < min) {
            min = data->elements[i];
        }
        if (max < data->elements[i]) {
            max = data->elements[i];
        }
    }
    uint16_t mid = ((max - min) / 2) + min;
    area->pivot = max;
    for (uint16_t i = start_index; i <= end_index; i++) {
        if (mid <= data->elements[i]) {
            if (data->elements[i] < area->pivot) {
                area->pivot = data->elements[i];
            }
        }
    }
//     for (int i = 0; i < pos->num_area; i++) {
//         if (pos->areas[i].alive == true) {
//             APP_LOG(APP_LOG_LEVEL_DEBUG, "s_area_set: alive: areas[%d] %d - %d", i, pos->areas[i].start_index, pos->areas[i].end_index);
//         }
//     }
    return area_index;
}

static int s_area_get_alive(SortPosition *pos, int now_area_index) {
    int index = -1;
    
    for (int i = 0; i < pos->num_area; i++) {
        if (i != now_area_index) {
            if (pos->areas[i].alive == true) {
                index = i;
                break;
            }
        }
    }
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "s_area_get_alive -> %d", index);
    return index;
}
