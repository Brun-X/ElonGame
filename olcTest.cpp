#define OLC_PGE_APPLICATION
#define __LINUX__

#include "olcPixelGameEngine.h"

struct SpritePosition
{
	int x;
	int y;
};

class Pico : public olc::PixelGameEngine
{
public:
	Pico()
	{
		sAppName = "Pico";
	}

	bool OnUserCreate() override
	{
		for(int x = 0; x < ScreenWidth(); x++)
		{
			for(int y = 0; y < ScreenHeight(); y++)
			{
				Draw(x, y, olc::DARK_YELLOW);
			}
		}

		SetPixelMode(olc::Pixel::MASK);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		if(GetKey(olc::Key::UP).bHeld) elonPos.y -= 2;
		if(GetKey(olc::Key::DOWN).bHeld) elonPos.y += 2;
		if(GetKey(olc::Key::RIGHT).bHeld) elonPos.x += 2;
		if(GetKey(olc::Key::LEFT).bHeld) elonPos.x -= 2;		

		//Draw Screen
		
		for(int x = 0; x < ScreenWidth(); x++)
		{
			for(int y = 0; y < ScreenHeight(); y++)
			{
				Draw(x, y, olc::DARK_YELLOW);
			}
		}

		DrawSprite(elonPos.x, elonPos.y, elon);

/*
		for(int x = elonPos.x; x < elonPos.x + 102; x++)
		{
			for(int y = elonPos.y; y < elonPos.y + 102; y++)
			{
				if(elon->GetPixel(x,y).a == 0) Draw(x, y, olc::DARK_YELLOW);
			}
		}
*/
		return true;
	}

public:
	olc::Sprite* elon = new olc::Sprite("lilElon.png");
	SpritePosition elonPos{ 0, 0 };
};



int main()
{
	Pico demo;
	if(demo.Construct(800, 600, 1, 1)) demo.Start();


	return 0;
}