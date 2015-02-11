#pragma once
#include <pebble.h>

#define COUNTDOWN_SECONDS 10
#define COUNTDOWN_ACTION_DELAY 3

void countdown_init();
void countdown_deinit();
void countdown_start();
