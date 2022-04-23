#include <stdint.h>
#include <stdio.h>
#include "viewdata_screen.h"

typedef struct {
	int row; //Row 0-23 of the cursor
	int col; //Column 0-39 for the cursor
	int cstate; //Cursor state: 0: invisible; 1: visible
	int tstate; //Terminal state: 0: normal, 1: ESC received, 2: APA received, 3: second APA character
	uint8_t cells[VD_ROWS][VD_COLS]; //character cells for this screen
} viewdata_screen_t;

viewdata_screen_t viewdata_screen;

int viewdata_set_cell(viewdata_screen_t *s, const uint8_t row, const uint8_t col, const uint8_t c)
{
	if (s==NULL) return -1;
	if (row>=VD_ROWS) return -1;
	if (col>=VD_COLS) return -1;
	s->cells[row][col]=c;
	return 0;
}

int viewdata_set_position(viewdata_screen_t *s, const uint8_t row, const uint8_t col)
{
	if (s==NULL) return -1;
	if (row>=VD_ROWS) return -1;
	if (col>=VD_COLS) return -1;
	s->row=row;
	s->col=col;
	return 0;
}


/*viewdata_move_position:
 * Moves the cursor and handles wrapping
 */
int viewdata_move_position(viewdata_screen_t *s, const int8_t row, const int8_t col)
{
	if (s==NULL) return -1;
	int nrow=s->row+row;
	int ncol=s->col+col;
	if (ncol<0) {
		ncol=VD_COLS-1;
		nrow=nrow-1;
	}
	if (ncol>=VD_COLS) {
		ncol=0;
		nrow=nrow+1;
	}
	if (nrow<0) {
		nrow=VD_ROWS-1;
	}
	if (nrow>=VD_ROWS) {
		nrow=0;
	}
	s->row=nrow;
	s->col=ncol;
	return 0;
}

int viewdata_print_character(viewdata_screen_t *s, const uint8_t c)
{
	if (s==NULL) return -1;

	viewdata_set_cell(s, s->row, s->col, c);
	return viewdata_move_position(s, 0, 1);
}

int viewdata_clear_screen(viewdata_screen_t *s)
{
	if (s==NULL) return -1;
	int y=0;
	//memset(s->cells, ' ', sizeof(s->cells));
	for (y=0; y<VD_ROWS; y++) {
		int x=0;
		for (x=0; x<VD_COLS; x++) {
			viewdata_set_cell(s, y, x, ' ');
		}
	}
	s->col=0;
	s->row=0;
	return 0;
}

int viewdata_cancel(viewdata_screen_t *s)
{
	if (s==NULL) return -1;
	int x=0;
	for (x=s->col; x<VD_COLS; x++) viewdata_set_cell(s,s->col, x, ' ');
	return 0;
}


int viewdata_handle_character(viewdata_screen_t *s, const uint8_t c)
{
	if (s==NULL) return -1;
	if (s->tstate==2) {
		s->row=c&0x1f;
		s->tstate=3;
		return -1;
	}
	if (s->tstate==3) {
		s->col=c&0x3f;
		s->tstate=0;
		return -1;
	}
	if (c==0x00) return 0;
	if (c==0x1B) {
		s->tstate=1; //ESC received
		return -1;
	}
	if ((s->tstate==1) || (c>=0x80)) {
		s->tstate=0;
		uint8_t c2=c&0x1f;
		if (c2<0x20) return viewdata_print_character(s, c2); //Print attribute character
		
	} else {
		if (c==0x08) return viewdata_move_position(s, 0,-1); //Active Position Backwards
		if (c==0x09) return viewdata_move_position(s, 0, 1); //Active Position Forward
		if (c==0x0A) return viewdata_move_position(s, 1, 0); //Active Position Down	
		if (c==0x0B) return viewdata_move_position(s,-1, 0); //Active Position Up
		if (c==0x0C) return viewdata_clear_screen(s); //Clear screen
		if (c==0x0D) return s->col=0; //Active Position Return
		if (c==0x11) return s->cstate=1; //Cursor on
		if (c==0x14) return s->cstate=0; //Cursor off
		if (c==0x18) return viewdata_cancel(s); //CANcel, clear line to end
		if (c==0x1E) { //Active Position Home
			s->col=0;
			s->row=0;
			return 0;
		}
		if ( (c>=0x20) && (c<=0x7f) ) return viewdata_print_character(s, c);
		printf("Undefined control character %02x\n", c);
	}
	return 0;
}

/*Initializes the screen*/
int viewdata_init_screen()
{
	viewdata_screen.col=0;
	viewdata_screen.row=0;
	viewdata_screen.cstate=1;
	viewdata_screen.tstate=0;
	return viewdata_clear_screen(&viewdata_screen);
}

/*Handles a string of input characters*/
int viewdata_handle_string(const uint8_t *c, const int len)
{
	if (c==NULL) return -1;
	if (len==0) return 0;
	int n;
	for (n=0; n<len; n++) viewdata_handle_character(&viewdata_screen, c[n]);
	return 0;
}

int viewdata_get_cell(const uint8_t row, const uint8_t col)
{
	if (row>=VD_ROWS) return -1;
	if (col>=VD_COLS) return -1;
	return viewdata_screen.cells[row][col];
}
