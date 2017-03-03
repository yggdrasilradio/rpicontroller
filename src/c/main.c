#include <pebble.h>

#define KEY_REQUEST 0
#define KEY_RESPONSE 1

static Window *window;
static MenuLayer *menu_layer;
static char response[128];

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return 6;
    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw text in the section header
	  if (strlen(response) == 0)
      	menu_cell_basic_header_draw(ctx, cell_layer, "RPILIGHTS");
	  else
		menu_cell_basic_header_draw(ctx, cell_layer, response);
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      // Use the row to specify which item we'll draw
      switch (cell_index->row) {
        case 0:
          menu_cell_basic_draw(ctx, cell_layer, "On", "Run last animation", NULL);
          break;
        case 1:
          menu_cell_basic_draw(ctx, cell_layer, "Off", "Turn all lights off", NULL);
          break;
        case 2: 
          menu_cell_basic_draw(ctx, cell_layer, "Weather", "Time/date/weather", NULL);
          break;
		case 3: 
          menu_cell_basic_draw(ctx, cell_layer, "Pacman", "Pacman animation", NULL);
          break;
		case 4: 
          menu_cell_basic_draw(ctx, cell_layer, "Rainbow", "Scrolling rainbow", NULL);
          break;
		case 5: 
          menu_cell_basic_draw(ctx, cell_layer, "Snow", "Snow animation", NULL);
          break;
      }
      break;
      }
  }

static void SendRequest(char *data) {
  DictionaryIterator *iter1;
  app_message_outbox_begin(&iter1);
  dict_write_cstring(iter1, KEY_REQUEST, data);
  app_message_outbox_send();
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	
  // Use the row to specify which item will receive the select action	
  switch (cell_index->row) {
    // This is the menu item with the cycling icon
    case 0:
	  SendRequest("on");
      break;
	case 1:
	  SendRequest("off");
      break;
	case 2:
	  SendRequest("weather");
      break;
	case 3:
	  SendRequest("pacman");
      break;
	case 4:
	  SendRequest("rainbow");
      break;
	case 5:
	  SendRequest("snow");
      break;
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  Tuple *t = dict_read_first(iterator);
  while (t != NULL) {

    switch(t->key) {
      case KEY_RESPONSE:
        snprintf(response, sizeof(response), "%s", t->value->cstring);
        layer_mark_dirty(menu_layer_get_layer(menu_layer));
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received: %s", response);
        break;
    }
    t = dict_read_next(iterator);
  }
}

static void window_load(Window *window) {
	
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
#ifdef PBL_COLOR
  menu_layer_set_highlight_colors(menu_layer, GColorFromRGB(0, 255, 255), GColorFromRGB(0, 0, 0));
#endif

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
	
}

static void window_unload(Window *window) {

  menu_layer_destroy(menu_layer);
	
}

static void init(void) {
	
	response[0] = '\0';
	  window = window_create();
	
		// Register callbacks
		app_message_register_inbox_received(inbox_received_callback);

		// Open AppMessage
		app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
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