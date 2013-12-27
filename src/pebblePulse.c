#include <pebble.h>

static Window *window;
static TextLayer *title;
static TextLayer *instructions;
static TextLayer *pulse_rate;
static TextLayer *copyright;
static int recording_pulse_rate;
static time_t t = 0;
static uint16_t t_ms = 0;
static char pulse_rate_string[256] = "";

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (recording_pulse_rate == 0){
    recording_pulse_rate = 1;
    time_ms(&t, &t_ms);
    text_layer_set_text(instructions, "Press after 10 beats");
    text_layer_set_text(pulse_rate, "");
  } else {
    recording_pulse_rate = 0;
    time_t t1 = 0;
    uint16_t t1_ms = 0;
    time_ms(&t1, &t1_ms);
    int elapsed_seconds = t1 - t;
    int elapsed_ms = t1_ms - t_ms;

    int elapsed_time = (int)(10 / (elapsed_seconds + ((double)elapsed_ms / 1000)) * 60);

    snprintf(pulse_rate_string, sizeof(pulse_rate_string), "Pulse: %d", elapsed_time);

    text_layer_set_text(pulse_rate, pulse_rate_string);
    text_layer_set_text(instructions, "Press Select to start");
  }

}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  recording_pulse_rate = 0;
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Initiate Text layers
  title = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(title, "Pebble Pulse");
  text_layer_set_text_alignment(title, GTextAlignmentCenter);
  text_layer_set_font(title, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(title));

  instructions = text_layer_create((GRect) { .origin = { 0, 20 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(instructions, "Press Select to start");
  text_layer_set_text_alignment(instructions, GTextAlignmentCenter);
  text_layer_set_font(instructions, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(instructions));

  pulse_rate = text_layer_create((GRect) { .origin = { 0, 50 }, .size = { bounds.size.w, 60 } });
  text_layer_set_text_alignment(pulse_rate, GTextAlignmentCenter);
  text_layer_set_font(pulse_rate, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  layer_add_child(window_layer, text_layer_get_layer(pulse_rate));

  copyright = text_layer_create((GRect) { .origin = { 0, 130 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(copyright, "github.com/tomwilshere");
  text_layer_set_text_alignment(copyright, GTextAlignmentCenter);
  text_layer_set_font(copyright, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_layer, text_layer_get_layer(copyright));
}

static void window_unload(Window *window) {
  text_layer_destroy(title);
  text_layer_destroy(instructions);
  text_layer_destroy(pulse_rate);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
