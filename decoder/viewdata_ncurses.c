/*This is an example frontent for the decoder. It interfaces to the decoder via viewdata_interface.h
 * This code is responsible for handling keyboard input and displaying/processing the screen buffer
 */

#include <curses.h>
#include <locale.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

#include "viewdata_screen.h"
#include "viewdata_interface.h"

void update_row(const int x, const int y, const int row, const int reveal)
{
	viewdata_decoded_cell_t cells[VD_COLS];
	viewdata_convert_row(row, cells);

	move(row+y, x);
	for (int col=0; col<VD_COLS; col++) {
		int bg=cells[col].bg;
		int fg=cells[col].fg;
		int blink=cells[col].blink;
		int glyph=cells[col].glyph;
		int cpn=bg*8+fg;
		attron(COLOR_PAIR(cpn));
		if (blink==1) attron(A_BLINK); else attroff(A_BLINK);
		const char *c=viewdata_glyph_to_utf8(glyph);
		if (c==NULL) printw("(%d)",glyph); else
		if ((reveal==0) && (cells[col].concealed==1)) printw(" "); else
		printw("%s", viewdata_glyph_to_utf8(glyph));
	}
}


void update_screen(const int x, const int y, int reveal)
{
	for (int row=0; row<VD_ROWS; row++) {
		update_row(x, y, row, reveal);
	}
	attron(COLOR_PAIR(7));
}

void print_debugdata(const int x, const int y)
{
	for (int row=0; row<VD_ROWS; row++) {
		move(row+y,x);
		for (int col=0; col<VD_COLS; col++) {
			int c=viewdata_get_cell(row, col);
			if (c==0x20) {
				attron(COLOR_PAIR(4));
				printw(" . ");
			} else
			if ( (c>=0x20) && (c<0x7f) ) {
				attron(COLOR_PAIR(2));
				printw(" %c ", c);
			} else {
				attron(COLOR_PAIR(1));
				printw("%02x ", c);
			}
		}
	}
	attron(COLOR_PAIR(7));
}




void init_colourpairs_(int bgnum, int bg)
{
	start_color();
	init_pair(0+bgnum*8, COLOR_BLACK,  bg);
	init_pair(1+bgnum*8, COLOR_RED,    bg);
	init_pair(2+bgnum*8, COLOR_GREEN,  bg);
	init_pair(3+bgnum*8, COLOR_YELLOW, bg);
	init_pair(4+bgnum*8, COLOR_BLUE,   bg);
	init_pair(5+bgnum*8, COLOR_MAGENTA,bg);
	init_pair(6+bgnum*8, COLOR_CYAN,   bg);
	init_pair(7+bgnum*8, COLOR_WHITE,  bg);
}

void init_colourpairs()
{
	init_colourpairs_(0, COLOR_BLACK);
	init_colourpairs_(1, COLOR_RED);
	init_colourpairs_(2, COLOR_GREEN);
	init_colourpairs_(3, COLOR_YELLOW);
	init_colourpairs_(4, COLOR_BLUE);
	init_colourpairs_(5, COLOR_MAGENTA);
	init_colourpairs_(6, COLOR_CYAN);
	init_colourpairs_(7, COLOR_WHITE);
}

void print_status(int x, int y, int reveal)
{
	move(y, x);
	struct timeval t;
	gettimeofday(&t, NULL);
	printw("Time: %06ld.%06ld; Press _ for Enter, F10 for Exit, F12 for debug", t.tv_sec, t.tv_usec);
	if (reveal==0) printw(" F1 for reveal"); else printw(" F1 for conceal");
}

int inputloop()
{
	int debug=0;
	int reveal=0;
	int changes=1;
	int ch=-1;
	while (0==0) {
		if (changes>0) {
			update_screen(0,0, reveal); //Only update screen if something has changed
			print_status(0,25, reveal);
			if (debug!=0) print_debugdata(0,26);
			refresh();
		}
		ch=getch();
		if (ch==KEY_F(1)) {
			reveal=(reveal+1)%2;
			changes=1;
		}else
		if (ch==KEY_F(10)) return 0; else
		if (ch==KEY_F(12)) {
			debug=1; 
			changes=1;
		}else
		changes=viewdata_handle_stuff(ch);
	}
}

int main(int argc, char *argv[])
{
	if (argc!=3) {
		fprintf(stderr, "Usage: %s <hostname> <port>\n\tFor example: %s nx.nxtel.org 23280\n", argv[0], argv[0]);
		return 1;
	}
	setlocale(LC_ALL, "");
	int res=0;
	res=viewdata_connect_and_init(argv[1], atoi(argv[2]));
	if (res<0) {
		fprintf(stderr, "Couldn't connect\n");
		return 0;
	}
	initscr();
	keypad(stdscr, TRUE);
	timeout(30);
	init_colourpairs();
	inputloop();

	endwin();
	return 0;
}
