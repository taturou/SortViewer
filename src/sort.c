#include <pebble.h>
#include "sort.h"

typedef struct sort {
    SortData data;
    SortAlgorithm algorithm;
    bool is_algorithm_opened;
    void *ctx;
    uint16_t ctx_size;
    uint16_t num_turn;
} Sort;

Sort *sort_create(const SortSettings settings) {
    Sort *sort = NULL;
    
    sort = calloc(sizeof(Sort), 1);
    if (sort != NULL) {
        sort->data.num_element = settings.num_element & 0xfffeU;
        sort->data.elements = calloc(sizeof(uint8_t), settings.num_element);
        sort->data.is_init = false;
        sort->is_algorithm_opened = false;
        if (sort->data.elements == NULL) {
            free(sort);
            sort = NULL;
        }
    }    
    return sort;
}

void sort_destroy(Sort *sort) {
    if (sort != NULL) {
        (void)sort_set_algorithm(sort, NULL);
        free(sort->data.elements);
        sort->data.elements = NULL;
        sort->data.num_element = 0;
        free(sort);
    }
}

SortData *sort_get_data(const Sort *sort) {
    SortData *data = NULL;
    
    if (sort != NULL) {
        data = (SortData*)&sort->data;
    }
    return data;
}

uint16_t sort_num_element(Sort *sort) {
    return sort->data.num_element;
}

uint16_t sort_num_turn(Sort *sort) {
    return sort->num_turn;
}

bool sort_set_algorithm(Sort *sort, SortAlgorithm *algorithm) {
    bool ret = false;

    if (sort != NULL) {
        // install
        if (algorithm != NULL) {
            if (sort->ctx == NULL) {
                memcpy(&sort->algorithm, algorithm, sizeof(SortAlgorithm));
                uint16_t ctx_size = (*sort->algorithm.get_ctx_size)();
                if (0 < ctx_size) {
                    sort->ctx = calloc(ctx_size, 1);
                    if (sort->ctx != NULL) {
                        sort->ctx_size = ctx_size;
                        sort->is_algorithm_opened = false;
                        ret = true;
                    }
                } else {
                    /* error: do nothing */
                }
            }
        // uninstall
        } else {
            if (sort->ctx != NULL) {
                if (sort->is_algorithm_opened == true) {
                    (*sort->algorithm.close)(sort->ctx, &sort->data);
                    sort->is_algorithm_opened = false;
                }
                free(sort->ctx);
                sort->ctx = NULL;
                sort->ctx_size = 0;
                memset(&sort->algorithm, 0, sizeof(SortAlgorithm));
                ret = true;
            }            
        }
    }
    return ret;
}

char *sort_get_algorithm_name(Sort *sort) {
    static char *name = "";

    if (sort != NULL) {
        if (sort->ctx != NULL) {
            name = (*sort->algorithm.get_name)();
        }
    }
    return name;
}

bool sort_init(Sort *sort, SortOrder order) {
    bool ret = false;
    
    if (sort != NULL) {
        SortData *data = &sort->data;

        switch (order) {
        case SO_AscendingOrder:
            for (int i = 1; i <= data->num_element; i++) {
                data->elements[i - 1] = i;
            }
            break;
        case SO_DescendingOrder:
            for (int i = 0; i < data->num_element; i++) {
                data->elements[i] = data->num_element - i;
            }
            break;
        case SO_Random:
            srand(time(NULL));
            memset(data->elements, 0, data->num_element);
            for (int val = 1; val <= data->num_element; val++) {
                uint16_t index = rand() % data->num_element;
                while (data->elements[index] != 0) {
                    index += rand() % data->num_element;
                    if (data->num_element <= index) {
                        index = 0;
                    }
                }
                data->elements[index] = val;
            }
            break;
        default:
            break;
        }
        
        if (sort->is_algorithm_opened == true) {
            (*sort->algorithm.close)(sort->ctx, &sort->data);
            sort->is_algorithm_opened = false;
        }
        ret = (*sort->algorithm.open)(sort->ctx, &sort->data);
        if (ret == true) {
            sort->is_algorithm_opened = true;
            sort->data.is_init = true;
            sort->num_turn = 0;
        }
    }
    return ret;
}

void sort_next(Sort *sort, bool *is_end) {
    if (sort != NULL) {
        if (sort->data.is_init == true) {
            (*sort->algorithm.next)(sort->ctx, &sort->data, is_end);
            if (*is_end == true) {
                sort->data.is_init = false;
            }
            sort->num_turn++;
        }
    }
}

void sort_draw(Sort *sort, GContext *gctx) {
    if ((sort != NULL)
        && (gctx != NULL)) {
        (*sort->algorithm.draw)(sort->ctx, &sort->data, gctx);
    }
}