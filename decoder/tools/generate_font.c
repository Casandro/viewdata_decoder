#include <stdio.h>
#include <stdint.h>

#define CHAR_HEIGHT (10)
#define CHAR_NUM_FILE (96)
#define CHAR_NUM (96+64+64)

uint8_t font[CHAR_HEIGHT*CHAR_NUM];


void read_font()
{
	//Read the font from file character by character
	FILE *f=fopen("mullard_SAA5050", "r");
	if (f==NULL) return;
	for (int c=0; c<CHAR_NUM_FILE; c++) {
		uint8_t rchar[10];
		fread(rchar, sizeof(rchar), 1, f);
		int fontpos=c*CHAR_HEIGHT; //Determine position in font
		//Shift and separate font for our file
		for (int line=0; line<10; line++)
		{
			font[fontpos+line]=rchar[line]<<1;
		}
	}
	fclose(f);
}

void print_font_char(int c)
{
	int fontpos=c*CHAR_HEIGHT;
	printf("\t{//Position: 0x%02x\n",c);
	for (int line=0; line<CHAR_HEIGHT; line++) {
		printf("\t\t0x%02x", font[fontpos+line]);
		if (line<CHAR_HEIGHT-1) printf(", "); else printf("  ");
		printf("//  ");
		for (int b=5; b>=0; b--) {
			int x=(font[fontpos+line]>>b) & 0x1;
			if (x==1) printf("#"); else printf(".");
		}
		printf("\n");
	}
	printf("\t}");
}

void print_font()
{
	int charnum=CHAR_NUM;
	printf("#include <stdint.h>\n");
	printf("#include \"font.h\"\n");
	printf("//File generated from tools/generate_font.c based on the Mullard SAA5050 ROM\n");
	printf("uint8_t vd_font[%d][%d]={\n", CHAR_NUM, CHAR_HEIGHT);
	for (int c=0; c<charnum; c++) {
		print_font_char(c);
		if (c<charnum-1) printf(",");
		printf("\n");
	}
	printf("\t};");
}

void gen_graphic_char(int c, int seg)
{
	int fontpos=c*CHAR_HEIGHT;
	for (int line=0; line<CHAR_HEIGHT; line++) font[fontpos+line]=0; //Emty character
	if ((seg&0x01)!=0) { //Left top segment
		for (int line=0; line<3; line++) font[fontpos+line]=font[fontpos+line]|0b111000;
	}
	if ((seg&0x02)!=0) { //right top segment
		for (int line=0; line<3; line++) font[fontpos+line]=font[fontpos+line]|0b000111;
	}
	if ((seg&0x04)!=0) { //Left middle segment
		for (int line=3; line<7; line++) font[fontpos+line]=font[fontpos+line]|0b111000;
	}
	if ((seg&0x08)!=0) { //right middle segment
		for (int line=3; line<7; line++) font[fontpos+line]=font[fontpos+line]|0b000111;
	}
	if ((seg&0x10)!=0) { //Left bottom segment
		for (int line=7; line<10; line++) font[fontpos+line]=font[fontpos+line]|0b111000;
	}
	if ((seg&0x20)!=0) { //right bottom segment
		for (int line=7; line<10; line++) font[fontpos+line]=font[fontpos+line]|0b000111;
	}
}

void set_segmented(int c)
{ //Clears bits to make mosaic char look segmented
	int fontpos=c*CHAR_HEIGHT;
	for (int line=0; line<CHAR_HEIGHT; line++) font[fontpos+line]=font[fontpos+line]&0b011011;
	font[fontpos+2]=0;
	font[fontpos+6]=0;
	font[fontpos+9]=0;
}


int main(int argc, char argv[])
{
	read_font();
	for (int gc=0; gc<64; gc++) gen_graphic_char(CHAR_NUM_FILE+gc, gc);
	for (int gc=0; gc<64; gc++) gen_graphic_char(CHAR_NUM_FILE+gc+64, gc);
	for (int gc=0; gc<64; gc++) set_segmented(CHAR_NUM_FILE+gc+64);
	print_font();
}
