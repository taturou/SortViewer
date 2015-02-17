#include <pebble.h>
#include <canvas.h>
#include <sort.h>
#include <sort_babble.h>

static Window *s_window;
static TextLayer *s_text_layer;
static Canvas *s_canvas;
static Sort *s_sort;

#define STR_LEN           (64)
static char s_str[STR_LEN];

#define TIMER_TIMEOUT_MS  (10)
static AppTimer *s_timer;

static void s_timer_callback_babble(void *data);

static void s_timer_start(void) {
    s_timer = app_timer_register(TIMER_TIMEOUT_MS, s_timer_callback_babble, NULL);
}

static void s_timer_stop(void) {
    snprintf(s_str, STR_LEN, "Stop (turn:%d)", sort_num_turn(s_sort));
    text_layer_set_text(s_text_layer, s_str);
    if (s_timer != NULL) {
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
        snprintf(s_str, STR_LEN, "Done (turn:%d)", sort_num_turn(s_sort));
    } else {
        s_timer_start();
        snprintf(s_str, STR_LEN, "Sort (turn:%d)", sort_num_turn(s_sort));
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

static void s_up_click_handler(ClickRecognizerRef recognizer, void *context) {
    s_timer_stop();
    snprintf(s_str, STR_LEN, "Init (num:%d)", sort_num_element(s_sort));
    text_layer_set_text(s_text_layer, s_str);
    (void)sort_init(s_sort, SORT_ALGORITHM_BABBLE_INIT_PARAM_RANDOM);
    canvas_mark_dirty(s_canvas);
}

static void s_down_click_handler(ClickRecognizerRef recognizer, void *context) {
    s_timer_stop();
    snprintf(s_str, STR_LEN, "Init (num:%d)", sort_num_element(s_sort));
    (void)sort_init(s_sort, SORT_ALGORITHM_BABBLE_INIT_PARAM_DESCORDER);
    canvas_mark_dirty(s_canvas);
}

static void s_click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, s_select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, s_up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, s_down_click_handler);
}

static void s_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect window_frame = layer_get_bounds(window_layer);

    s_sort = sort_create((SortSettings){.num_element = window_frame.size.w / 2});
    (void)sort_set_algorithm(s_sort, &sort_algorithm_babble);
    
    s_canvas = canvas_create((GRect){.origin = {0, 20}, .size = {window_frame.size.w, window_frame.size.h - 20}}, s_sort);
    layer_add_child(window_layer, canvas_get_layer(s_canvas));

    s_text_layer = text_layer_create((GRect){.origin={0, 0}, .size={window_frame.size.w, 20}});
    text_layer_set_text(s_text_layer, "Load");
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void s_window_unload(Window *window) {
    text_layer_destroy(s_text_layer);
    s_text_layer = NULL;

    canvas_destroy(s_canvas);
    s_canvas = NULL;
    
    sort_destroy(s_sort);
    s_sort = NULL;
    
    s_timer_stop();
}

static void s_init(void) {
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