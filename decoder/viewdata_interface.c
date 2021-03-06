#include <stddef.h>
#include "viewdata_interface.h"

int viewdata_connect_and_init(const char *ip, int port)
{
	int res=0;
	res=viewdata_init_screen();
	if (res<0) return res;
	res=viewdata_connect(ip, port);
	if (res<0) return res;
	return res;
};

int viewdata_handle_stuff(const int input)
{
	return viewdata_handle(input);
};

const char *vd_glyph_to_utf8[0xA0]={
//      x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xA   xB   xC   xD   xE   xF
/*0x */	" ", "!","\"", "#", "ยค", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
/*1x */	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
/*2x */	"@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
/*3x */	"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[","\\", "]", "^", "_",
/*4x */	"`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
/*5x */	"p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "โ ",
/*6x */	" ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ",
/*7x */	"๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "โ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ", "๐ฌ",
/*8x*/	"๐ฌ", "๐ฌ", "๐ฌ ", "๐ฌก", "๐ฌข", "๐ฌฃ", "๐ฌค", "๐ฌฅ", "๐ฌฆ", "๐ฌง", "โ", "๐ฌจ", "๐ฌฉ", "๐ฌช", "๐ฌซ", "๐ฌฌ",
/*9x*/	"๐ฌญ", "๐ฌฎ", "๐ฌฏ", "๐ฌฐ", "๐ฌฑ", "๐ฌฒ", "๐ฌณ", "๐ฌด", "๐ฌต", "๐ฌถ", "๐ฌท", "๐ฌธ", "๐ฌน", "๐ฌบ", "๐ฌป", "โ"};

const char *viewdata_glyph_to_utf8(const int glyph)
{
	if (glyph<0) return NULL;
	if (glyph>0xA0) return NULL;
	return vd_glyph_to_utf8[glyph];
}


void viewdata_convert_row(const int row, viewdata_decoded_cell_t cells[])
{
	if (cells==NULL) return;
	if (row<0) return;
	int dh_low=0; //0: lower row of a double height row
	if (row>0) { //Find out of there were double height characters in the previous row
		for (int col=0; col<VD_COLS; col++) {
			int c=viewdata_get_cell(row-1, col);
			if (c==0x0d) { //Double height
				dh_low=1;
				break;
			}
		}
	}
	int mosaic=0;
	int last_mosaic_glyph=-1;
	int hold_mosaic=0;
	int blink=0;
	int underline=0; //Underline/segmented
	int concealed=0;
	int fg=7;
	int bg=0;
	int size=0;
	for (int col=0; col<VD_COLS; col++) {
		//Fixme: Handle hold mosaics
		int c=-1;
		if (dh_low==0) c=viewdata_get_cell(row, col);
		else c=viewdata_get_cell(row-1, col);



		//Set-at codes
		if (c==0x1d) bg=fg; //New Background
		if (c==0x1c) bg=0; //Black background

		if (c==0x0c) { //normal height
			if (size!=0) last_mosaic_glyph=0;
			size=0; //normal height
		}

		if (c==0x1e) { //hold mosaics on
			hold_mosaic=1;
		}
		if (c==0x19) underline=1; //Separated graphics/underline
		if (c==0x1A) underline=1; //Continuous graphics
		if (c==0x18) concealed=1; //Concealed

		cells[col].blink=blink;
		cells[col].fg=fg;
		cells[col].bg=bg;
		cells[col].underline=underline;
		cells[col].concealed=concealed;
		
		if ( (size==0) && (dh_low==1) ) {
			//This is a lower double height row, but normal sized characters
			//blank cell
			cells[col].glyph=0;
		} else {
			if (dh_low==0) cells[col].size=size; //If upper row, set to normal or upper
			else cells[col].size=2; //Otherwise set to lower half
			
			if (c<0x20) { //attribute characters
				if (hold_mosaic<=0) cells[col].glyph=0; //...lead to spaces if no hold mosaic
				else { //If hold mosaic, use previous glyph
					if ((last_mosaic_glyph>0) ) {
						cells[col].glyph=last_mosaic_glyph;
					} else cells[col].glyph=0;
				}
			} else if (c>=0x80) {
				//High bit set
				cells[col].glyph=0; // set to spaces for now
			} else {
				if (mosaic==0) cells[col].glyph=c-0x20; else {
					//mosaik=1
					int c2=c-0x20;
					if (c2<0x20) cells[col].glyph=0x60+c2; else //Lower block graphics
					if (c2<0x40) cells[col].glyph=c2; else //Upper case letters
					if (c2<0x60) cells[col].glyph=0x80+(c2-0x40); //Upper blockgraphics
					last_mosaic_glyph=cells[col].glyph;
				}
			}
		}
		//Set-After codes
		if (c==0x1f) hold_mosaic=0; //release mosaics
		//Colour codes
		if ( ((c>=0x00) && (c<=0x07)) ||
		     ((c>=0x10) && (c<=0x17))) {
			fg=c&0x07;
			if (c>=0x10) {
				if (mosaic!=1) last_mosaic_glyph=0;
				mosaic=1;
			}
		       	else {
				if (mosaic!=0) last_mosaic_glyph=0;
				mosaic=0;
			}
			concealed=0; //Colour codes reveal text
		}
		if (c==0x08) blink=1; //Flash
		if (c==0x09) blink=0; //Steady
		if (c==0x0d) { //double height
			if (size!=1) last_mosaic_glyph=0;
			size=1;
		}
	}
}
