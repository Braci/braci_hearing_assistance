#include "paging.h"

static Window *wnd;
static MenuLayer *menu;

static void menu_draw_header(GContext *ctx, const Layer *layer, uint16_t section_idx, void *cb_ctx) {
}
static void menu_draw_row(GContext *ctx, const Layer *layer, MenuIndex *cell_idx, void *cb_ctx) {
}
static uint16_t menu_get_num_sections(struct MenuLayer *ml, void *ctx) {
}
static uint16_t menu_get_num_rows(struct MenuLayer *ml, uint16_t section_idx, void *cb_ctx) {
}
static void menu_select_click(struct MenuLayer *ml, MenuIndex *idx, void *cb_ctx) {
}

static void window_load(Window *wnd) {
	Layer *root = window_get_root_layer(wnd);
	GRect bounds = layer_get_bounds(wnd_layer);

	menu = menu_layer_create(bounds);
	menu_layer_set_callbacks(menu, NULL, (MenuLayerCallbacks) {
		.draw_header = menu_draw_header,
		.draw_row = menu_draw_row,
		.get_num_sections = menu_get_num_sections,
		.get_num_rows = menu_get_num_rows,
		.select_click = menu_select_click,
	});
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
