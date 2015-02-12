#include "paging.h"
#include "events.h"

static Window *wnd;
static MenuLayer *menu;

static void menu_draw_header(GContext *ctx, const Layer *layer, uint16_t section_idx, void *cb_ctx) {
	if(section_idx == 0)
		return;
	menu_cell_basic_header_draw(ctx, layer, "Settings");
}
static void menu_draw_row(GContext *ctx, const Layer *layer, MenuIndex *idx, void *cb_ctx) {
	if(idx->section == 0) {
		int evt = PAGING_INDICES[idx->row];
		menu_cell_basic_draw(ctx, layer,
				NOTIFY_TEXTS[evt], NULL, NULL);
	} else {
		menu_cell_basic_draw(ctx, layer,
				"Fall detection",
				app_worker_is_running() ? "Enabled" : "Disabled",
				NULL);
	}
}
static int16_t menu_get_header_height(struct MenuLayer *ml, uint16_t section_idx, void *cb_ctx) {
	if(section_idx == 0)
		return 0;
	else
		return MENU_CELL_BASIC_HEADER_HEIGHT;
}
static uint16_t menu_get_num_sections(struct MenuLayer *ml, void *ctx) {
	return 2;
}
static uint16_t menu_get_num_rows(struct MenuLayer *ml, uint16_t section_idx, void *cb_ctx) {
	if(section_idx == 0)
		return ARRAY_LENGTH(PAGING_INDICES);
	else
		return 1;
}
static void menu_select_click(struct MenuLayer *ml, MenuIndex *idx, void *cb_ctx) {
	if(idx->section == 1) {
		if(app_worker_is_running()) {
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Stopping worker");
			app_worker_kill();
		} else {
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Launching worker");
			app_worker_launch();
		}
		psleep(100); // for worker to stop/launch; FIXME: it's dirty!
		menu_layer_reload_data(ml);
		return;
	}

	int event = PAGING_INDICES[idx->row];
	// TODO: send event
	window_stack_remove(wnd, false);
}

static void window_load(Window *wnd) {
	Layer *root = window_get_root_layer(wnd);
	GRect bounds = layer_get_bounds(root);

	menu = menu_layer_create(bounds);
	menu_layer_set_callbacks(menu, NULL, (MenuLayerCallbacks) {
		.draw_header = menu_draw_header,
		.draw_row = menu_draw_row,
		.get_header_height = menu_get_header_height,
		.get_num_sections = menu_get_num_sections,
		.get_num_rows = menu_get_num_rows,
		.select_click = menu_select_click,
	});
	menu_layer_set_click_config_onto_window(menu, wnd);
	layer_add_child(root, menu_layer_get_layer(menu));
}
static void window_unload(Window *wnd) {
	menu_layer_destroy(menu);
}

void paging_open() {
	window_stack_push(wnd, true);
}

void paging_init() {
	wnd = window_create();
	window_set_window_handlers(wnd, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});
}
void paging_deinit() {
	window_destroy(wnd);
}
