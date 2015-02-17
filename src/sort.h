#pragma once
#include <pebble.h>

typedef struct sort Sort;

typedef struct sort_settings {
    uint8_t num_element;
} SortSettings;

typedef struct sort_data {
    uint8_t num_element;
    uint8_t *elements;
    bool is_init;
} SortData;

typedef struct sort_algorithm {
    uint16_t (*get_ctx_size)(void);
    bool (*open)(void *ctx, SortData *data, void *param);
    void (*close)(void *ctx, SortData *data);
    void (*next)(void *ctx, SortData *data, bool *is_end);
    void (*draw)(void *ctx, SortData *data, GContext *gctx);
} SortAlgorithm;

Sort *sort_create(const SortSettings settings);
void sort_destroy(Sort *sort);
SortData *sort_get_data(const Sort *sort);
uint16_t sort_num_element(Sort *sort);
uint16_t sort_num_turn(Sort *sort);
bool sort_set_algorithm(Sort *sort, SortAlgorithm *algorithm);
bool sort_init(Sort *sort, void *param);
void sort_next(Sort *sort, bool *is_end);
void sort_draw(Sort *sort, GContext *gctx);
