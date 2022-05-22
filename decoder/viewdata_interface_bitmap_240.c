#include "viewdata_interface_bitmap_240.h"
#include "viewdata_interface.h"
#include "viewdata_screen.h"
#include "font.h"

void viewdata_intf_set_bp_char(const int col, const uint8_t bits_, uint8_t *bplane)
{
	//Sets the 6 bits of a bitplane
	if (col>=VD_COLS) return;
	uint8_t bits=bits_&0b111111;
	int ppos=col*VD_FONT_WIDTH;
	int octet=ppos/8;
	int obit=ppos%8; //Bits numered from most significant to least significant 0=>0x80 7=>0x01

	//0123456789abcdef //16 bits in b
	//    xxxxxx       //Bits to overwrite obit=4
	//fedcba9876543210 //shift amount
	uint8_t bh=bplane[octet];
	uint8_t bl=bplane[octet+1];
	uint16_t b=(bh<<8)|bl;
	int bshift=10-obit;	
	b=b&(~(0b111111<<bshift));
	b=b|    (bits<<bshift);
	bplane[octet]  =(b>>8);
	bplane[octet+1]=(b&0xff);
}

void viewdata_intf_set_bp_char_fgbg(const int col, const uint8_t bits, const int fg, const int bg, uint8_t *bplane)
{
	if ( (fg==0) && (bg==0) ) //If neither foreground or background have this bitplane, set it to 0
		return viewdata_intf_set_bp_char(col, 0, bplane);
	if ( (fg!=0) && (bg!=0) ) //If both foreground and background have this bitplane set, fill the block
		return viewdata_intf_set_bp_char(col, 0b111111, bplane);
	if ( fg!=0) //If only fireground is set, set the bits accordingly
		return viewdata_intf_set_bp_char(col, bits, bplane);
	if ( bg!=0) //If ony background is set, invert the bits
		return viewdata_intf_set_bp_char(col, 0b111111^bits, bplane);
	return; //We should never reach this
}

void viewdata_intf_set_char_color(const int col, const uint8_t bits, const uint8_t fg, const uint8_t bg, uint8_t *bp_red, uint8_t *bp_green, uint8_t *bp_blue)
{
	viewdata_intf_set_bp_char_fgbg(col, bits, (fg&0x1), (bg%0x1), bp_red);
	viewdata_intf_set_bp_char_fgbg(col, bits, (fg&0x2), (bg%0x2), bp_green);
	viewdata_intf_set_bp_char_fgbg(col, bits, (fg&0x4), (bg%0x4), bp_blue);
}

void viewdata_intf_set_character(const viewdata_decoded_cell_t *cell, const int col, const int reveal, const int blink, vd_240_line_t *bitplanes)
{
	int glyph=cell->glyph;
	if ((cell->glyph>=0x60) && (cell->glyph<=0x9F)) {
		if (cell->underline!=0) glyph=cell->glyph+64; //select a separated mosaic glyph
	}
	if ( (cell->blink!=1) && (blink!=0) ) glyph=0; //Blank character for blinking
	if ( (cell->concealed!=0) && (reveal==0) ) glyph=0; //Blank character for reveal

	for (int line=0; line<VD_FONT_HEIGHT; line++) {
		uint8_t bits=vd_font[glyph][line];
		//Handle double height
		if (cell->size==1) bits=vd_font[glyph][line/2];
		if (cell->size==2) bits=vd_font[glyph][line/2+VD_FONT_HEIGHT/2];
		int bp_pos=line*VD_BM_LLEN;
		viewdata_intf_set_char_color(col, bits, cell->fg, cell->bg, &(bitplanes->red[bp_pos]), &(bitplanes->green[bp_pos]),&(bitplanes->blue[bp_pos]));
	}
}

void viewdata_240_update_row(const int row, const int reveal, const int blink, vd_240_line_t *line)
{
	viewdata_decoded_cell_t cells[VD_COLS];
	viewdata_convert_row(row, cells);
	for (int col=0; col<VD_COLS; col++) {
		viewdata_intf_set_character(&(cells[col]), col, reveal, blink, line);
	}
}

/*
Example for usage:

#include <stdio.h>

const char test_color[8]=" RGYBMCW";

//This is an example for a function that gets a line from the viewdata screen and converts it into pixels
void test_print_240_line(const int row, const int reveal, const int blink)
{
	vd_240_line_t line;
	//First zero the memory (should be optional)
	for (int col=0; col<VD_BM_BP_LEN; col++) {
		line.red[col]=0l;
		line.green[col]=0;
		line.blue[col]=0;
	}
	//Call the routine that generates the bitplanes
	viewdata_240_update_row(row, reveal, blink, &line);
	//For each of the 10 lines of a charactter line
	for (int pline=0; pline<VD_FONT_HEIGHT; pline++) {
		//Go through all the bytes
		for (int col=0; col<VD_BM_LLEN; col++) {
			int bppos=col+pline*VD_BM_LLEN;
			uint8_t red=line.red[bppos];
			uint8_t green=line.green[bppos];
			uint8_t blue=line.blue[bppos];
			//And go through the bits of the bytes and output the coloured pixel
			for (int n=0; n<8; n++) {
				int c=((red>>7)&1) | (((green>>7)&1)<<1) | (((blue>>7)&1)<<2);
				red=red<<1; green=green<<1; blue=blue<<1;
				printf("%c", test_color[c]);
			}
		}
		printf("\n");
	}
}

//Do the above for every line
void test_print_240()
{
	for (int row=0; row<VD_ROWS; row++) {
		test_print_240_line(row,1,0);
	}
}

//This is a bad example for an event loop
int test_240_inputloop()
{
	int debug=0;
	int reveal=0;
	int changes=1;
	int ch=-1;
	while (0==0) {
		if (changes>0) {
			test_print_240();
		}
		changes=viewdata_handle_stuff(-1);
		sleep(1);
	}
}

*/
