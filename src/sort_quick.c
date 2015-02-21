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

typedef struct sort_position {
} SortPosition;

static char *s_get_name(void) {
    static char *name = "Quick";
    return name;
}

static uint16_t s_get_ctx_size(void) {
    return sizeof(SortPosition);
}

static bool s_open(void *ctx, SortData *data) {
//    SortPosition *pos = (SortPosition*)ctx;
    
    return true;
}

static void s_close(void *ctx, SortData *data) {
    (void)ctx;
    (void)data;    
}

static void s_next(void *ctx, SortData *data, bool *is_end) {
//    SortPosition *pos = (SortPosition*)ctx;

}

static void s_draw(void *ctx, SortData *data, GContext *gctx) {
//    SortPosition *pos = (SortPosition*)ctx;
}