#include <pebble.h>
#include <menu.h>
#include <canvas.h>
#include <sort.h>
#include <sort_babble.h>
#include <sort_merge.h>
#include <sort_quick.h>
#include <sort_bucket.h>

static Window *s_window;
static Menu *s_menu;
static TextLayer *s_text_layer;
static Canvas *s_canvas;
static Sort *s_sort;

#define STR_LEN           (64)
static char s_str[STR_LEN];

#define TIMER_TIMEOUT_MS  (20)
static AppTimer *s_timer;

#define DELAY_MENU        (500)
#define DELAY_BUTTON_DOWN (50)

static void s_timer_callback_babble(void *data);

static void s_timer_start(void) {
    s_timer = app_timer_register(TIMER_TIMEOUT_MS, s_timer_callback_babble, NULL);
}

static void s_timer_stop(void) {
    text_layer_set_text(s_text_layer, s_str);
    if (s_timer != NULL) {
        snprintf(s_str, STR_LEN, "%s:Stop(#%d)", sort_get_algorithm_name(s_sort), sort_num_turn(s_sort));
        app_timer_cancel(s_timer);
        s_timer = NULL;
    }
}

static bool s_timer_running(void) {
    return s_timer == NULL ? false : true;
}

static void s_timer_callback_babble(void *data) {
    bool is_end = true;
    
    sort_next(s_sort, &is_end);
    if (is_end == true) {
        s_timer_stop();
        snprintf(s_str, STR_LEN, "%s:Done(#%d)", sort_get_algorithm_name(s_sort), sort_num_turn(s_sort));
    } else {
        s_timer_start();
        snprintf(s_str, STR_LEN, "%s:Sort(#%d)", sort_get_algorithm_name(s_sort), sort_num_turn(s_sort));
    }
    text_layer_set_text(s_text_layer, s_str);
    canvas_mark_dirty(s_canvas);
}

static void s_select_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (s_timer_running() == true) {
        s_timer_stop();
    } else {
        s_timer_start();
    }
    canvas_mark_dirty(s_canvas);
}

static void s_menu_select_callback(AlgorithmKind kind) {
    SortAlgorithm *algorithm = NULL;

    switch (kind) {
    case MSA_Babble:
        algorithm = &sort_algorithm_babble;
        break;
    case MSA_Merge:
        algorithm = &sort_algorithm_merge;
        break;
    case MSA_Quick:
        algorithm = &sort_algorithm_quick;
        break;
    case MSA_Bucket:
        algorithm = &sort_algorithm_bucket;
        break;
    default:
        break;
    }
    if (algorithm != NULL) {
        (void)sort_set_algorithm(s_sort, NULL);
        (void)sort_set_algorithm(s_sort, algorithm);
        (void)sort_init(s_sort,  SO_DescendingOrder);
        canvas_set_time(s_canvas);
        snprintf(s_str, STR_LEN, "%s:Init(num:%d)", sort_get_algorithm_name(s_sort), sort_num_element(s_sort));
    }
    canvas_mark_dirty(s_canvas);
}

static void s_select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
    s_timer_stop();
    menu_show(s_menu, s_menu_select_callback);
}

static void s_up_click_handler(ClickRecognizerRef recognizer, void *context) {
    s_timer_stop();
    snprintf(s_str, STR_LEN, "%s:Init(num:%d)", sort_get_algorithm_name(s_sort), sort_num_element(s_sort));
    text_layer_set_text(s_text_layer, s_str);
    (void)sort_init(s_sort, SO_Random);
    canvas_set_time(s_canvas);
    canvas_mark_dirty(s_canvas);
}

static void s_down_click_handler(ClickRecognizerRef recognizer, void *context) {
    bool is_end = true;

    s_timer_stop();
    sort_next(s_sort, &is_end);
    if (is_end == true) {
        snprintf(s_str, STR_LEN, "%s:Done(#%d)", sort_get_algorithm_name(s_sort), sort_num_turn(s_sort));
    } else {
        snprintf(s_str, STR_LEN, "%s:Sort(#%d)", sort_get_algorithm_name(s_sort), sort_num_turn(s_sort));
    }
    text_layer_set_text(s_text_layer, s_str);
    canvas_mark_dirty(s_canvas);
}

static void s_click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, s_select_click_handler);
    window_long_click_subscribe(BUTTON_ID_SELECT, DELAY_MENU, s_select_long_click_handler, NULL);
    window_single_click_subscribe(BUTTON_ID_UP, s_up_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, DELAY_BUTTON_DOWN, s_down_click_handler);
}

static void s_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_bounds(window_layer);

    s_menu = menu_create();
    
    s_sort = sort_create((SortSettings){.num_element = 64 /*window_frame.size.w / 2*/});
    (void)sort_set_algorithm(s_sort, &sort_algorithm_babble);
    (void)sort_init(s_sort, SO_AscendingOrder);

    s_canvas = canvas_create((GRect){.origin = {0, 20}, .size = {window_frame.size.w, window_frame.size.h - 20}}, s_sort);
    canvas_set_time(s_canvas);
    layer_add_child(window_layer, canvas_get_layer(s_canvas));

    s_text_layer = text_layer_create((GRect){.origin={0, 0}, .size={window_frame.size.w, 20}});
    text_layer_set_text(s_text_layer, "Load");
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void s_window_unload(Window *window) {
    s_timer_stop();

    canvas_destroy(s_canvas);
    s_canvas = NULL;
    
    sort_destroy(s_sort);
    s_sort = NULL;
    
    menu_destroy(s_menu);
    s_menu = NULL;

    text_layer_destroy(s_text_layer);
    s_text_layer = NULL;
}

static void s_init(void) {
    s_menu = NULL;
    s_text_layer = NULL;
    s_canvas = NULL;
    s_sort = NULL;
    s_timer = NULL;

    s_window = window_create();
    window_set_fullscreen(s_window, true);
    window_set_click_config_provider(s_window, s_click_config_provider);
    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = s_window_load,
        .unload = s_window_unload,
    });
    const bool animated = true;
    window_stack_push(s_window, animated);
}

static void s_deinit(void) {
    window_destroy(s_window);
}

int main(void) {
    s_init();
    app_event_loop();
    s_deinit();
}