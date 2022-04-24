#pragma once

#include "viewdata_network.h"
#include "viewdata_screen.h"

typedef struct {
	unsigned int glyph:8; //Glyph of the character; See viewdata_glyph_to_utf8
	unsigned int fg:3; //Foreground colour 0:black, 1:red, 2:green, 3:yellow, 4:blue, 5:magenta, 6:cyan, 7:white 
	unsigned int bg:3; //Background colour 0:black, 1:red, 2:green, 3:yellow, 4:blue, 5:magenta, 6:cyan, 7:white 
	unsigned int blink:1; //blink mode 0:stead, 1:blink
	unsigned int size:2; //0:regular size, 1:double size top, 2: double size bottom
	unsigned int underline:1; //0:regular, 1:underline/separated graphics
	unsigned int concealed:1; //0:visible, 2:only visible after reveal pressed
} viewdata_decoded_cell_t;


int viewdata_connect_and_init(const char *ip, int port);
int viewdata_handle_stuff(const int input);
const char *viewdata_glyph_to_utf8(const int glyph);
void viewdata_convert_row(const int row, viewdata_decoded_cell_t cells[]);

