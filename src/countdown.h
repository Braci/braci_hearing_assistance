#pragma once
#include <pebble.h>

// count N seconds before action
#define COUNTDOWN_SECONDS 10
// show message for M seconds
#define COUNTDOWN_MESSAGE_DELAY 1

void countdown_init();
void countdown_deinit();
void countdown_start(int event, bool immediately);
