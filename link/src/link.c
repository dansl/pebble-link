#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x15, 0x3C, 0xE0, 0xA8, 0xCC, 0xE1, 0x42, 0x17, 0xB1, 0x5B, 0xA6, 0xE8, 0x0C, 0x70, 0x09, 0x5A }
PBL_APP_INFO(MY_UUID,
             "Link", "Dansl",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
BmpContainer background_image;

TextLayer text_date_layer;
TextLayer text_time_layer;

void update_display(PblTm *tick_time) {
  static char date_text[] = "Xxx, Xxx 00";
  static char new_date_text[] = "Xxx, Xxx 00";
  static char time_text[] = "00:00";
  char *time_format;

  // Date layer
  string_format_time(new_date_text, sizeof(date_text), "%a, %b %d", tick_time);
  if (strncmp(new_date_text, date_text, sizeof(date_text)) != 0) {
      strncpy(date_text, new_date_text, sizeof(date_text));
      text_layer_set_text(&text_date_layer, date_text);
  }

  // Time layer
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }
  string_format_time(time_text, sizeof(time_text), time_format, tick_time);
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(&text_time_layer, time_text);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;

  update_display(t->tick_time);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Link");
  window_stack_push(&window, true /* Animated */);

  resource_init_current_app(&APP_RESOURCES);

  bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image);
  layer_add_child(&window.layer, &background_image.layer.layer);

  // Date Layer
  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(0, 40, 144, 168-88));
  text_layer_set_text_alignment(&text_date_layer, GTextAlignmentCenter);
  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PIXEL_18)));
  layer_add_child(&window.layer, &text_date_layer.layer);

  // Time Layer
  GRect time_rect;
  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(0, 5, 144, 168-54));
  text_layer_set_text_alignment(&text_time_layer, GTextAlignmentCenter);
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PIXEL_36)));
  layer_add_child(&window.layer, &text_time_layer.layer);


  // No blank screen on init.
  PblTm tick_time;
  get_time(&tick_time);
  update_display(&tick_time);
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&background_image);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}