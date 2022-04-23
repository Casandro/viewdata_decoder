/*This is an example frontent for the decoder. It interfaces to the decoder via viewdata_interface.h
 * This code is responsible for handling keyboard input and displaying/processing the screen buffer
 */

#include <curses.h>
#include <locale.h>
#include <string.h>
#include <sys/time.h>

#include "viewdata_screen.h"
#include "viewdata_interface.h"

char *tt_to_utf8[2][96]={
	{
	" ", "!","\"", "#", "¤", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
	"@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
	"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[","\\", "]", "^", "_",
	"`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
	"p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "■"},
	{
	" ", "🬀", "🬁", "🬂", "🬃", "🬄", "🬅", "🬆", "🬇", "🬈", "🬉", "🬊", "🬋", "🬌", "🬍", "🬎",
	"🬏", "🬐", "🬑", "🬒", "🬓", "▌", "🬔", "🬕", "🬖", "🬗", "🬘", "🬙", "🬚", "🬛", "🬜", "🬝",
	"@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
	"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "←", "½", "→", "↑", "⌗",
	"🬞", "🬟", "🬠", "🬡", "🬢", "🬣", "🬤", "🬥", "🬦", "🬧", "▐", "🬨", "🬩", "🬪", "🬫", "🬬",
	"🬭", "🬮", "🬯", "🬰", "🬱", "🬲", "🬳", "🬴", "🬵", "🬶", "🬷", "🬸", "🬹", "🬺", "🬻", "█"}};

void update_row(const int x, const int y, const int row)
{
	int bg=0;
	int fg=7;
	int mosaic=0; //0: text; 1: mosaic; 2: separated graphics
	int col=0;

	move(row+y, x);
	for (col=0; col<VD_COLS; col++) {
		//Fixme: Handle blinking and hold mosaics
		int c=viewdata_get_cell(row, col);
		if ( ((c>=0x00) && (c<=0x07)) ||
		     ((c>=0x10) && (c<=0x17))) {
			fg=c&0x07;
			if (c>=0x10) mosaic=1; else mosaic=0;
		}
		if (c==0x1d) bg=fg; //New Background
		if (c==0x1c) bg=0; //Black background

		int cpn=bg*8+fg;
		attron(COLOR_PAIR(cpn));
		
		if (c<0x20) {
			printw(" ", c);
		} else if (c<0x80) printw("%s", tt_to_utf8[mosaic][c-32]);
		else {
			printw("X");
		}
	}
}

void update_screen(const int x, const int y)
{
	int row=0;
	for (row=0; row<VD_ROWS; row++) {
		update_row(x, y, row);
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

void print_status(int x, int y)
{
	move(y, x);
	struct timeval t;
	gettimeofday(&t, NULL);
	printw("Time: %06ld.%06ld; Press _ for Enter, F10 for Exit", t.tv_sec, t.tv_usec);
}

int inputloop()
{
	int ch=-1;
	while (0==0) {
		update_screen(0,0);
		print_status(0,25);
		refresh();
		ch=getch();
		if (ch==KEY_F(10)) return 0; else
		viewdata_handle_stuff(ch);
	}
}

int main()
{
	setlocale(LC_ALL, "");
	int res=0;
	res=viewdata_connect_and_init("51.68.195.248", 23280);
	if (res<0) {
		fprintf(stderr, "Couldn't connect\n");
		return 0;
	}
	initscr();
	keypad(stdscr, TRUE);
	timeout(300);
	init_colourpairs();
	inputloop();

	endwin();
	return 0;
}
