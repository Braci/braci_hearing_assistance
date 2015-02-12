#pragma once
#include <pebble.h>

// ms
#define SPLASH_TIMEOUT 1000

void splash_init();
void splash_deinit();

void splash_send(int event);
