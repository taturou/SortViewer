#include <pebble.h>
#include "menu.h"

#define NUM_MENU_SECTIONS       (1)
#define NUM_MENU_SECTION1_ROWS  (MAX_ALGORITHM_KIND)

typedef struct menu {
    Window *window;
    MenuLayer *layer;
    MenuIndex selected_index;
    MenuSelectCallback callback;
}Menu;

static void s_window_load(Window *window);
static void s_window_unload(Window *window);

Menu *menu_create(void) {
    Menu *menu = NULL;

    menu = calloc(sizeof(Menu), 1);
    if (menu != NULL) {
        menu->selected_index = (MenuIndex){0, 0};
    }
    return menu;
}

void menu_destroy(Menu *menu) {
    if (menu != NULL) {
        menu_hide(menu);
        free(menu);
    }
}

bool menu_show(Menu *menu, MenuSelectCallback callback) {
    bool ret = false;

    if (menu != NULL) {
        Window *window = window_create();
        if (window != NULL) {
            menu->window = window;
            menu->callback = callback;

            // init window
            window_set_background_color(window, GColorWhite);
            window_set_user_data(window, (void*)menu);
            window_set_window_handlers(window, (WindowHandlers) {
                .load = s_window_load,
                .unload = s_window_unload,
            });
            window_stack_push(window, true /* Animated */);
            ret = true;
        }
    }
    return ret;
}

bool menu_hide(Menu *menu) {
    bool ret = false;

    if (menu != NULL) {
        if (menu->window != NULL) {
            window_stack_pop(true);
            window_destroy(menu->window);
            menu->window = NULL;
            ret = true;
        }
    }
    return ret;
}

static uint16_t s_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
    return NUM_MENU_SECTIONS;
}

static uint16_t s_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    const uint16_t num_rows[NUM_MENU_SECTIONS] = {
        NUM_MENU_SECTION1_ROWS,
    };
    return num_rows[section_index];
}

static int16_t s_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    // This is a define provided in pebble.h that you may use for the default height
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void s_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
    const char *titles[NUM_MENU_SECTIONS] = {
        "Algorithm"
    };
    menu_cell_basic_header_draw(ctx, cell_layer, titles[section_index]);
}

static void s_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    (void)data;    
    struct basic_cell {
        char *title;
        char *sub_title;
        GBitmap *icon;
    };
    const struct basic_cell cells1[NUM_MENU_SECTION1_ROWS] = {
        {"Babble sort", NULL, NULL},
        {"Merge sort", NULL, NULL},
        {"Quick sort", NULL, NULL},
        {"Bucket sort", NULL, NULL}
    };
    const struct basic_cell *cells[NUM_MENU_SECTIONS] = {
        cells1
    };
    const struct basic_cell *cell = &cells[cell_index->section][cell_index->row];
    menu_cell_basic_draw(ctx, cell_layer, cell->title, cell->sub_title, cell->icon);
}

static void s_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    Menu *menu = (Menu*)data;

    menu->selected_index = *cell_index;
    (*menu->callback)((AlgorithmKind)cell_index->row);
    menu_hide(menu);
}

static void s_window_load(Window *window) {
    Menu *menu = window_get_user_data(window);
    
    Layer *window_layer = window_get_root_layer(window);

    menu->layer = menu_layer_create(layer_get_frame(window_layer));
    menu_layer_set_callbacks(menu->layer, (void*)menu, (MenuLayerCallbacks){
        .get_num_sections = s_menu_get_num_sections_callback,
        .get_num_rows = s_menu_get_num_rows_callback,
        .get_header_height = s_menu_get_header_height_callback,
        .draw_header = s_menu_draw_header_callback,
        .draw_row = s_menu_draw_row_callback,
        .select_click = s_menu_select_callback,
    });
    menu_layer_set_click_config_onto_window(menu->layer, window);
    menu_layer_set_selected_index(menu->layer, menu->selected_index, MenuRowAlignCenter, false);
    layer_add_child(window_layer, menu_layer_get_layer(menu->layer));
}

static void s_window_unload(Window *window) {
    Menu *menu = window_get_user_data(window);

    menu_layer_destroy(menu->layer);
    menu->layer = NULL;
}