
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
	float x;
	float y;
	float w;
	float h;
	float velX = 0.0f;
	float velY = 0.0f;
	float acc = 0.0f;
	bool onGround = false;
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
		levelTiles += "#........#.....................#";
		levelTiles += "#........#.....................#";
		levelTiles += "#........#.#...................#";
		levelTiles += "#.########.##..................#";
		levelTiles += "#........#.###.................#";
		levelTiles += "#........#.####................#";
		levelTiles += "###......#...########..#.......#";
		levelTiles += "#........##..#.........#.......#";
		levelTiles += "#............#.........#.......#";
		levelTiles += "#..##########.......####.......#";
		levelTiles += "#..............#####...........#";
		levelTiles += "#.......######.####............#";
		levelTiles += "#......##....#.................#";
		levelTiles += "#............######............#";
		levelTiles += "#..............................#";
		levelTiles += "################################";

		brick = new olc::Sprite(16, 16);
		player = new olc::Sprite(8, 8);

		for(int x = 0; x < ScreenWidth(); x++)
		{
			for(int y = 0; y < ScreenHeight(); y++)
			{
				Draw(x, y, olc::CYAN);
			}
		}

		SetPixelMode(olc::Pixel::NORMAL);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//User input

		if(GetKey(olc::Key::SPACE).bPressed) 
		{
			if(playerPos.velY == 0.0f && playerPos.onGround) playerPos.velY = -12.0f * tileHeight;
		}
		//if(GetKey(olc::Key::UP).bHeld) playerPos.y -= 2;
		//if(GetKey(olc::Key::DOWN).bHeld) playerPos.y += 2;

		if(GetKey(olc::Key::RIGHT).bHeld) playerPos.velX += 15.0f * tileWidth * fElapsedTime * 1.5f;
		if(GetKey(olc::Key::LEFT).bHeld) playerPos.velX += -15.0f * tileWidth * fElapsedTime * 1.5f;


		

		//physics

		//gravity
		playerPos.velY += 20.0f * tileHeight * (fElapsedTime * 1.5f);


		//drag
		if(playerPos.onGround)
		{
			playerPos.velX += -3.0f * playerPos.velX * fElapsedTime * 1.5f;
			if(fabs(playerPos.velX) < 0.01f) playerPos.velX = 0.0f;
		}
		

		//clamping velocity
		playerPos.velX = playerPos.velX < -8.0f * tileWidth ? -8.0f * tileWidth : playerPos.velX;
		playerPos.velX = playerPos.velX > 8.0f * tileWidth ? 8.0f * tileWidth : playerPos.velX;

		playerPos.velY = playerPos.velY < -20.0f * tileHeight ? -20.0f * tileHeight : playerPos.velY;
		playerPos.velY = playerPos.velY > 10.0f * tileHeight ? 10.0f * tileHeight : playerPos.velY;


		float potNewX = playerPos.x + playerPos.velX * fElapsedTime * 1.5f;
		float potNewY = playerPos.y + playerPos.velY * fElapsedTime * 1.5f;

		// Left
		if(playerPos.velX <= 0)
		{
			int xTile = (int)(potNewX) / tileWidth;
			int yTile = (int)(playerPos.y) / tileHeight;
			int yTileOffset = (int)(playerPos.y + playerPos.h - 1) / tileHeight;
			if(levelTiles[yTile * levelWidth + xTile] == '#' || levelTiles[yTileOffset * levelWidth + xTile] == '#')
			{
				potNewX = xTile == 0 ? tileWidth : xTile * tileWidth + ((int)potNewX % (xTile * tileWidth)) + 1;
				playerPos.velX = 0.0f;
			}
		}
		// Right
		else
		{
			int xTileOffset = (int)(potNewX + playerPos.w) / tileWidth;
			int yTile = (int)(potNewY) / tileHeight;
			int yTileOffset = (int)(potNewY + playerPos.h - 1) / tileHeight;
			if(levelTiles[yTile * levelWidth + xTileOffset] == '#' || levelTiles[yTileOffset * levelWidth + xTileOffset] == '#')
			{
				potNewX = (int)potNewX;
				playerPos.velX = 0.0f;
			}
		}

		playerPos.onGround = false;

		// Up
		if(playerPos.velY <= 0.0f)
		{
			int xTile = (int)(potNewX) / tileWidth;
			int xTileOffset = (int)(potNewX + playerPos.w - 1) / tileWidth;
			int yTile = (int)(potNewY) / tileHeight;
			if(levelTiles[yTile * levelWidth + xTile] == '#' || levelTiles[yTile * levelWidth + xTileOffset] == '#')
			{
				potNewY = (int)(yTile * tileHeight + tileHeight);
				playerPos.velY = 0.0f;
			}
		}
		// Down
		else
		{
			int xTile = (int)(potNewX) / tileWidth;
			int xTileOffset = (int)(potNewX + playerPos.w - 1) / tileWidth;
			int yTileOffset = (int)(potNewY + playerPos.h) / tileHeight;
			if(levelTiles[yTileOffset * levelWidth + xTile] == '#' || levelTiles[yTileOffset * levelWidth + xTileOffset] == '#')
			{
				potNewY = (int)potNewY;
				playerPos.velY = 0.0f;
				playerPos.onGround = true;
			}
		}

		playerPos.x = potNewX;
		playerPos.y = potNewY;
		

		//Snap Sprite

		/*
		float tileX = GetMouseX() / tileWidth;
		float tileY = GetMouseY() / tileHeight;

		int snapX = (tileX - (int)tileX) < 0.5 ? (int)tileX : (int)tileX + 1; 
		int snapY = (tileY - (int)tileY) < 0.5 ? (int)tileY : (int)tileY + 1; 

		playerPos.x = (snapX * tileWidth) - (playerPos.w / 2);
		playerPos.y = (snapY * tileHeight) - (playerPos.h / 2);
		*/
		




		//Draw Screen
		for(int x = 0; x < levelWidth; x++)
		{
			for(int y = 0; y < levelHeight; y++)
			{
				olc::Pixel p = levelTiles[y * levelWidth + x] == '#' ? olc::DARK_RED : olc::CYAN;
				FillRect(x * tileWidth, y * tileHeight, tileWidth, tileHeight, p);
			}
		}

		DrawSprite(playerPos.x, playerPos.y, player);

		return true;
	}

public:
	std::vector<olc::Sprite*> tiles;
	olc::Sprite* brick;
	olc::Sprite* player;
	SpritePosition playerPos {(float)tileWidth + 2, 100.0f, 8.0f, 8.0f, 0.0f, 0.0f, 0.0f, false};
};



int main()
{
	Pico demo;
	if(demo.Construct(256, 256, 4, 4)) demo.Start();


	return 0;
}