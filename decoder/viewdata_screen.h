#pragma once
#include <stdint.h>

#define VD_COLS (40)
#define VD_ROWS (24)

int viewdata_init_screen();
int viewdata_handle_string(const uint8_t *c, const int len);
int viewdata_get_cell(const uint8_t row, const uint8_t col);
