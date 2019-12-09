#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

#define SCREEN_WIDTH 48
#define SCREEN_HEIGHT 48

WINDOW* this_window;

/*
--------
--------
--++++--
--++++--
--++++--
--++++--
--------
--------
*/

struct Square
{
	int Xbuffer[1024];
	int Ybuffer[1024];
	int height;
	int length;
	//int lengthOfBuffer = 0;
};

void create_square(struct Square* s, int height, int length, int posY, int posX)
{
	int bufferSizeCounter = 0;
	for(int y = 0; y < height; y++)
	{
		s->Ybuffer[y] = y + posY;
		for(int x = 0; x < length; x++)
		{	
			s->Xbuffer[x * y+1] = x + posX;
			bufferSizeCounter++;
		}

	}
	s->height = height;
	s->length = length;
	//s->lengthOfBuffer = bufferSizeCounter;
}

void draw_objects(int offsetY, int offsetX, int length, int height, WINDOW * w)
{	
	for(int y = offsetY; y < (height + offsetY); y++)
	{
		for(int x = offsetX; x < (length + offsetX); x++)
		{
			move(y, x);
			printw("#");
		}
	}

}


int main(int argc, char** argv)
{
	initscr();
	this_window = newwin(SCREEN_HEIGHT, SCREEN_WIDTH, 0, 0);

	draw_objects(10, 10, 8, 8, this_window);

	refresh();
	getch();
	delwin(this_window);
	endwin();
	return 0;
}