
/*

IMPLEMNTED WITH THE OLCPIXELGAMEENGINE FROM:
		    
		    www.onelonecoder.com

		  GREAT THANKS TO JAVIDX9

*/


#define OLC_PGE_APPLICATION
#define __LINUX__

#include "olcPixelGameEngine.h"





struct SpritePosition
{
	int x;
	int y;
	int w;
	int h;
};

class Pico : public olc::PixelGameEngine
{
private:
	int levelWidth;
	int levelHeight;
	std::string levelTiles;
	int tileWidth = 8;
	int tileHeight = 8;

public:
	Pico()
	{
		sAppName = "Pico";
	}

	bool OnUserCreate() override
	{
		levelWidth = 32;
		levelHeight = 32;
		levelTiles += "################################";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "#..............................#";
		levelTiles += "################################";

		elon = new olc::Sprite("lilElon.png");

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
		//User input

		if(GetKey(olc::Key::UP).bHeld) elonPos.y -= 2;
		if(GetKey(olc::Key::DOWN).bHeld) elonPos.y += 2;
		if(GetKey(olc::Key::RIGHT).bHeld) elonPos.x += 2;
		if(GetKey(olc::Key::LEFT).bHeld) elonPos.x -= 2;		
		
		//Collision detection

		//float colX = elonPos.x / tileWidth;
		//float colY = elonPos.y / tileHeight;

		float colOffsetXLeft = elonPos.x / tileWidth;
		float colOffsetXRight = (elonPos.x + elonPos.w) / tileWidth;

		float colOffsetYTop = elonPos.y / tileHeight;
		float colOffsetYBottom = (elonPos.y + elonPos.h) / tileHeight;

		elonPos.x = colOffsetXLeft < 1 ? (tileWidth + 1) : elonPos.x;
		elonPos.x = colOffsetXRight > (levelWidth - 1) ? (tileWidth * (levelWidth - 1)) - elonPos.w : elonPos.x;

		elonPos.y = colOffsetYTop < 1 ? (tileHeight + 1) : elonPos.y;
		elonPos.y = colOffsetYBottom > (levelHeight - 1) ? (tileHeight * (levelHeight - 1)) - elonPos.h : elonPos.y;

		//Snap Sprite

		/*
		float tileX = GetMouseX() / tileWidth;
		float tileY = GetMouseY() / tileHeight;

		int snapX = (tileX - (int)tileX) < 0.5 ? (int)tileX : (int)tileX + 1; 
		int snapY = (tileY - (int)tileY) < 0.5 ? (int)tileY : (int)tileY + 1; 

		elonPos.x = (snapX * tileWidth) - (elonPos.w / 2);
		elonPos.y = (snapY * tileHeight) - (elonPos.h / 2);
		*/
		


		//Draw Screen


		
		for(int x = 0; x < levelWidth; x++)
		{
			for(int y = 0; y < levelHeight; y++)
			{
				olc::Pixel p = levelTiles[y * levelWidth + x] == '#' ? olc::DARK_RED : olc::BLUE;
				FillRect(x * tileWidth, y * tileHeight, tileWidth, tileHeight, p);
			}
		}

		DrawSprite(elonPos.x, elonPos.y, elon);

		return true;
	}

public:
	std::vector<olc::Sprite*> tiles;
	olc::Sprite* elon;
	SpritePosition elonPos {60, 60, 102, 102};
};



int main()
{
	Pico demo;
	if(demo.Construct(256, 256, 4, 4)) demo.Start();


	return 0;
}