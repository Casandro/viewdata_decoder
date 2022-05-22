#pragma once
#include <stdint.h>
#include "viewdata_screen.h"
#include "font.h"

//Width of the whole screen in pixels
#define VD_BM_WIDTH (VD_COLS*VD_FONT_WIDTH)
//Height of the whole screen in pixels
#define VD_BM_HEIGHT (VD_ROWS*VD_FONT_HEIGHT)

//Length of a line in a bitplane
#define VD_BM_LLEN (VD_BM_WIDTH/8)
//Length of a bitplane in the following buffer
#define VD_BM_BP_LEN (VD_BM_LLEN*VD_FONT_HEIGHT)


//This structure holds the bitplanes for a text line of display (10 pixel lines)
//Pixels are encoded with the most significant bit representing the left most pixel
//So 0x80 represents 1.......
//   0x01 represents .......1
//Each bitplane (red, green and blue) represents one colour component
typedef struct {
	uint8_t red[VD_BM_BP_LEN];
	uint8_t green[VD_BM_BP_LEN];
	uint8_t blue[VD_BM_BP_LEN];
} vd_240_line_t;

void viewdata_240_update_row(const int row, const int reveal, const int blink, vd_240_line_t *line);

//int test_240_inputloop();
