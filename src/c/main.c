#include <pebble.h>

#define KEY_REQUEST 0
#define KEY_RESPONSE 1

static Window *window;
static MenuLayer *menu_layer;
static TextLayer *text_layer;
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

	return 0;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {

	//switch (section_index) {
		//case 0:
			//if (strlen(response) == 0)
				//menu_cell_basic_header_draw(ctx, cell_layer, "RPILIGHTS");
			//else
				//menu_cell_basic_header_draw(ctx, cell_layer, response);
			//break;
	//}
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {

	switch (cell_index->section) {
		case 0:
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
	
	switch (cell_index->row) {
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
				text_layer_set_text(text_layer, response);
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Received: %s", response);
				break;
		}
		t = dict_read_next(iterator);
	}
}

static void window_load(Window *window) {
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	bounds.origin.y += MENU_CELL_BASIC_HEADER_HEIGHT;
	bounds.size.h -= MENU_CELL_BASIC_HEADER_HEIGHT;
	menu_layer = menu_layer_create(bounds);
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	bounds = layer_get_bounds(window_layer);
	bounds.size.h = MENU_CELL_BASIC_HEADER_HEIGHT;
	text_layer = text_layer_create(bounds);
	text_layer_set_text_color(text_layer, GColorFromRGB(255, 255, 255));
	text_layer_set_background_color(text_layer, GColorFromRGB(0, 0, 255));
	
#ifdef PBL_COLOR
	menu_layer_set_highlight_colors(menu_layer, GColorFromRGB(0, 255, 255), GColorFromRGB(0, 0, 0));
#endif
	menu_layer_set_click_config_onto_window(menu_layer, window);
	text_layer_set_text(text_layer, "RPILIGHTS");
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {

	menu_layer_destroy(menu_layer);
}

static void init(void) {
	
	response[0] = '\0';
	window = window_create();
	app_message_register_inbox_received(inbox_received_callback);
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
