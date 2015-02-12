#pragma once
#include <pebble.h>

void accel_init();
void accel_deinit();

void accel_start();
void accel_stop();
bool accel_is_running();
void accel_pause();
void accel_unpause();
