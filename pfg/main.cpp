
/*

IMPLEMNTED WITH THE OLCPIXELGAMEENGINE FROM:
		    
		    www.onelonecoder.com

		  GREAT THANKS TO JAVIDX9

*/


#define OLC_PGE_APPLICATION
#define __LINUX__

#include "olcPixelGameEngine.h"
#include <boost/shared_ptr.hpp>





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
	float spriteFrameCounter = 0.0f;
	int spriteNo = 0;
	std::vector<boost::shared_ptr<olc::Sprite>> sprites;
};

class Pico : public olc::PixelGameEngine
{
private:
	int levelWidth;
	int levelHeight;
	std::string levelTiles;
	int tileWidth = 16;
	int tileHeight = 16;
	int cameraX = 8;
	int camPadding = 8;
	int FoVwidth;

	std::vector<boost::shared_ptr<olc::Sprite>> levelSprites;

public:
	Pico()
	{
		sAppName = "Pico";
	}

	bool OnUserCreate() override
	{
		levelWidth = 64;
		levelHeight = 15;
		levelTiles += "################################################################";
		levelTiles += "#........#.#..............................#....................#";
		levelTiles += "#.########.##...........#######...........#.....#..............#";
		levelTiles += "#........#.###.......................##..#........#............#";
		levelTiles += "#........#.####.......................#..#.....#...............#";
		levelTiles += "###......#...########..#.........##...#..#...........#.........#";
		levelTiles += "#........##..#.........#..............#..#...#.................#";
		levelTiles += "#............#.........#......##......#..................#.....#";
		levelTiles += "#..##########.......####..............#...#....................#";
		levelTiles += "#..............#####.......##.........########.................#";
		levelTiles += "#........#####.####............................................#";
		levelTiles += "#......##....#...........##..................................#.#";
		levelTiles += "#............######............................................#";
		levelTiles += "#..............................................................#";
		levelTiles += "################################################################";

		levelSprites.push_back(boost::shared_ptr<olc::Sprite>(new olc::Sprite("brick_tile.png")));
		playerPos.sprites.push_back(boost::shared_ptr<olc::Sprite>(new olc::Sprite("pikman_right_1.png")));
		playerPos.sprites.push_back(boost::shared_ptr<olc::Sprite>(new olc::Sprite("pikman_right_2.png")));

		for(int x = 0; x < ScreenWidth(); x++)
		{
			for(int y = 0; y < ScreenHeight(); y++)
			{
				Draw(x, y, olc::CYAN);
			}
		}

		SetPixelMode(olc::Pixel::ALPHA);

		//camPadding = 8 * tileWidth;
		FoVwidth = ScreenWidth() / tileWidth;
		//cameraX = playerPos.x < camPadding ? camPadding : playerPos.x;
		//cameraX = playerPos.x > levelWidth * tileWidth - camPadding ? levelWidth * tileWidth - camPadding : playerPos.x; 

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

		playerPos.spriteFrameCounter += fElapsedTime;
		if(playerPos.spriteFrameCounter > 0.125f)
		{
			playerPos.spriteNo++;
			playerPos.spriteNo %= 2;
			playerPos.spriteFrameCounter -= 0.125f; 
		}
		

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
			int yTile = (int)(playerPos.y) / tileHeight;
			int yTileOffset = (int)(playerPos.y + playerPos.h - 1) / tileHeight;
			if(levelTiles[yTile * levelWidth + xTileOffset] == '#' || levelTiles[yTileOffset * levelWidth + xTileOffset] == '#')
			{
				potNewX = ((int)potNewX / tileWidth) * tileWidth;
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
				potNewY = ((int)potNewY / tileHeight) * tileHeight;
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
	
		//cameraX = playerPos.x < (FoVwidth / 2) * tileWidth ? FoVwidth / 2 : (int)playerPos.x / tileWidth;
		//cameraX = playerPos.x + tileWidth > (levelWidth - FoVwidth / 2) * tileWidth ? levelWidth - FoVwidth / 2 : (int)playerPos.x / tileWidth;
		cameraX = playerPos.x / tileWidth;
		//int fovOffsetX = FoVwidth / 2;

		//if(playerPos.velX > 0) cameraX += 1;

		int xOffsetCam = cameraX - FoVwidth / 2;
		int tileOffsetX = ((int)playerPos.x % tileWidth);

		if(xOffsetCam < 0)
		{ 
			xOffsetCam = 0;
			tileOffsetX = 0;
		}
		if(xOffsetCam >= levelWidth - FoVwidth)
		{
			xOffsetCam = levelWidth - FoVwidth;
			tileOffsetX = 0;
		}
		


		//Draw Screen
		int xOffset = 0;
		for(int x = -1; x <= FoVwidth; x++)
		{
			for(int y = 0; y < levelHeight; y++)
			{
				int levelSpriteNo = levelTiles[y * levelWidth + x + xOffsetCam] == '#' ? 0 : 1;
				//olc::Pixel p = levelTiles[y * levelWidth + x + xOffsetCam] == '#' ? olc::DARK_RED : olc::CYAN;
				if(levelSpriteNo == 1) FillRect(x * tileWidth - tileOffsetX, y * tileHeight, tileWidth, tileHeight, olc::CYAN);
				else if(levelSpriteNo == 0) DrawSprite(x * tileWidth - tileOffsetX, y * tileHeight, levelSprites[0].get());
			}
			xOffset++;
		}




		/*
		for(int x = 0; x < levelWidth; x++)
		{
			for(int y = 0; y < levelHeight; y++)
			{
				olc::Pixel p = levelTiles[y * levelWidth + x] == '#' ? olc::DARK_RED : olc::CYAN;
				FillRect(x * tileWidth, y * tileHeight, tileWidth, tileHeight, p);
			}
		}
		*/

		DrawSprite(playerPos.x - (tileWidth * xOffsetCam) - tileOffsetX, (playerPos.y), playerPos.sprites[playerPos.spriteNo].get());

		return true;
	}

public:
	std::vector<olc::Sprite*> tiles;
	olc::Sprite* brick;
	olc::Sprite* player;
	SpritePosition playerPos {(float)tileWidth + 2, 100.0f, 16.0f, 16.0f, 0.0f, 0.0f, 0.0f, false};
};



int main()
{
	Pico demo;
	if(demo.Construct(256, 240, 4, 4)) demo.Start();


	return 0;
}