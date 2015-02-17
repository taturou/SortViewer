#include <pebble.h>

static Window *s_window;
static TextLayer *s_text_layer;

typedef struct {
  Layer *sc_layer;
  GRect sc_frame;
} SCanvas;
static SCanvas s_canvas;

typedef struct {
  uint16_t sd_size;
  uint16_t *sd_data;
} SData;
static SData s_data;

typedef struct {
  uint16_t ssb_in;
  uint16_t ssb_out;
} SSortBabble;
static SSortBabble s_babble;

#define TIMER_TIMEOUT_MS  (10)
static AppTimer *s_timer;

static inline void s_data_draw_line(GContext *ctx, uint16_t index) {
  GPoint p0;
  GPoint p1;

  if (index < s_data.sd_size) {
    p0.x = p1.x = index * 2;
    p0.y = s_canvas.sc_frame.size.h;
    p1.y = p0.y - s_data.sd_data[index];
    
    graphics_draw_line(ctx, p0, p1);
  }
}

static inline void s_data_draw_index(GContext *ctx) {
  GPoint p0;
  GPoint p1;
  
  // out
  p0.x = 0;
  p1.x = s_babble.ssb_out * 2;
  p0.y = p1.y = 20;
  graphics_draw_line(ctx, p0, p1);

  // in
  p0.x = 0;
  p1.x = s_babble.ssb_in * 2;
  p0.y = p1.y = 23;
  graphics_draw_line(ctx, p0, p1);
}

static void s_layer_update_callback(struct Layer *layer, GContext *ctx) {
  (void)layer;

  // set color
  graphics_context_set_stroke_color(ctx, GColorBlack);

  // draw line
  for (int i = 0; i < s_data.sd_size; i++) {
    s_data_draw_line(ctx, i);
  }
  
  // draw index
  s_data_draw_index(ctx);
}

static inline void s_data_swap(uint16_t index1, uint16_t index2) {
  uint16_t tmp = s_data.sd_data[index1];
  s_data.sd_data[index1] = s_data.sd_data[index2];
  s_data.sd_data[index2] = tmp;
}

static void s_timer_callback_babble(void *data) {
  if (s_data.sd_data[s_babble.ssb_in] < s_data.sd_data[s_babble.ssb_in - 1]) {
    s_data_swap(s_babble.ssb_in, s_babble.ssb_in - 1);
  }
  s_babble.ssb_in++;
  if (s_babble.ssb_out <= s_babble.ssb_in) {
    s_babble.ssb_in = 1;
    s_babble.ssb_out--;
  }
  if (s_babble.ssb_out == 1) {
    app_timer_cancel(s_timer);
    s_timer = NULL;
    text_layer_set_text(s_text_layer, "Babble Sorted!");
  } else {
    s_timer = app_timer_register(TIMER_TIMEOUT_MS, s_timer_callback_babble, NULL);
  }
  layer_mark_dirty(s_canvas.sc_layer);
}

static void s_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_timer != NULL) {
    text_layer_set_text(s_text_layer, "Babble Sort");
    app_timer_cancel(s_timer);
    s_timer = NULL;
  } else {
    text_layer_set_text(s_text_layer, "Babble Sorting...");
    s_timer = app_timer_register(TIMER_TIMEOUT_MS, s_timer_callback_babble, NULL);
  }
  layer_mark_dirty(s_canvas.sc_layer);
}

static void s_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Babble Sort");
  if (s_timer != NULL) {
    app_timer_cancel(s_timer);
    s_timer = NULL;
  }
  srand(time(NULL));
  memset(s_data.sd_data, 0, s_data.sd_size * sizeof(uint16_t));
  for (int val = 1; val <= s_data.sd_size; val++) {
    uint16_t index = rand() % s_data.sd_size;
    while (s_data.sd_data[index] != 0) {
      index++;
      if (s_data.sd_size <= index) {
        index = 0;
      }
    }
    s_data.sd_data[index] = val * 2;
  }
  s_babble.ssb_in = 1;
  s_babble.ssb_out = s_data.sd_size;
  layer_mark_dirty(s_canvas.sc_layer);
}

static void s_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Babble Sort");
  if (s_timer != NULL) {
    app_timer_cancel(s_timer);
    s_timer = NULL;
  }
  for (int i = 0; i < s_data.sd_size; i++) {
    s_data.sd_data[i] = (s_data.sd_size - i) * 2;
  }
  s_babble.ssb_in = 1;
  s_babble.ssb_out = s_data.sd_size;
  layer_mark_dirty(s_canvas.sc_layer);
}

static void s_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, s_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, s_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, s_down_click_handler);
}

static void s_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_frame = layer_get_bounds(window_layer);

  // create canvas
  s_canvas.sc_layer = layer_create(window_frame);
  s_canvas.sc_frame = window_frame;

  // create data
  s_data.sd_size = window_frame.size.w / 2;
  s_data.sd_data = malloc(s_data.sd_size * sizeof(uint16_t));
  memset(s_data.sd_data, 0, s_data.sd_size);
  
  layer_set_update_proc(s_canvas.sc_layer, s_layer_update_callback);
  layer_add_child(window_layer, s_canvas.sc_layer);
  
  // debug
  s_text_layer = text_layer_create((GRect){.origin={0, 0}, .size={window_frame.size.w, 20}});
  text_layer_set_text(s_text_layer, "Babble Sort");
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void s_window_unload(Window *window) {
  layer_destroy(s_canvas.sc_layer);
  free(s_data.sd_data);
}

static void s_init(void) {
  s_canvas.sc_layer = NULL;
  s_canvas.sc_frame = (GRect){.origin={0,0}, .size={0,0}};
  s_data.sd_data = NULL;
  s_data.sd_size = 0;
  s_babble.ssb_in = 0;
  s_babble.ssb_out = 0;
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