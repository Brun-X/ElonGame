
/*

IMPLEMNTED WITH THE OLCPIXELGAMEENGINE FROM:
		    
		    www.onelonecoder.com

		  GREAT THANKS TO JAVIDX9

*/


#define OLC_PGE_APPLICATION
#define __LINUX__

#include "olcPixelGameEngine.h"
#include <boost/shared_ptr.hpp>
#include <string>





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
	std::vector<boost::shared_ptr<olc::Sprite>> enemySprites;
	std::vector<boost::shared_ptr<olc::Sprite>> levelSprites;
	std::vector<boost::shared_ptr<SpritePosition>> enemyPos;
	int noEnemys = 0;
	bool gameOver = false, reset_sequence = false;
	float deadCounter = 0.0f;

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
		levelTiles += "#.....................e........................................#";
		levelTiles += "##########aa############a#aaaa#####aaaaa########aaaaaaaaaaaa####";

		levelSprites.push_back(boost::shared_ptr<olc::Sprite>(new olc::Sprite("brick_tile.png")));
		levelSprites.push_back(boost::shared_ptr<olc::Sprite>(new olc::Sprite("acid_tile.png")));

		enemySprites.push_back(boost::shared_ptr<olc::Sprite>(new olc::Sprite("enemy1.png")));
		playerPos.sprites.push_back(boost::shared_ptr<olc::Sprite>(new olc::Sprite("pikman_right_1.png")));
		playerPos.sprites.push_back(boost::shared_ptr<olc::Sprite>(new olc::Sprite("pikman_right_2.png")));

		size_t enemyIdx = 0;
		size_t eoffset = 0;
		do
		{
			enemyIdx = levelTiles.find('e', eoffset);
			if(enemyIdx != std::string::npos) 
			{
				enemyPos.push_back(boost::shared_ptr<SpritePosition>(new SpritePosition{(float)((enemyIdx % levelWidth) * tileWidth), (float)((enemyIdx / levelWidth) * tileHeight), 16, 16}));
				eoffset = enemyIdx + 1;
			}
		}
		while(enemyIdx != std::string::npos);

		//std::cout << enemyPos[0]->x << ", " << enemyPos[0]->y << std::endl;

		noEnemys = enemyPos.size();

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

		auto reset_game = [=](int colorState)
		{
			for(int x = 0; x <= FoVwidth; x++)
			{
				short unsigned int red = 0, green = 0, blue = 0;

				int scalar = colorState / 4;

				red = ((scalar + 1) % 2) * ((colorState * x) / 2) + 63;
				green = (((scalar + 2) / 2) % 2) * ((colorState * x) / 2) + 63;
				blue = ((scalar + 1) / 4) * ((colorState * x) / 2) + 63;

/*
				red = ((((2 + colorState) * x) / 4) + 16) % 128;
				green = ((((4 + colorState) * x) / 8) + 32) % 128;
				blue = ((((8 + colorState) * x) / 16) + 48) % 128;
*/
				olc::Pixel p(red, green, blue);
				for(int y = 0; y < levelHeight; y++)
				{
					FillRect(x * tileWidth, y * tileHeight, tileWidth, tileHeight, p);
				}
			}

			DrawString((ScreenWidth() / 2) - 48, 88, "Game Over!", olc::BLUE, 2);
			return true;
		};

		//User input
		static float potNewX = playerPos.x, potNewY = playerPos.y;
		if(!gameOver)
		{

			if(GetKey(olc::Key::SPACE).bPressed) 
			{
				if(playerPos.velY == 0.0f && playerPos.onGround) playerPos.velY = -12.0f * tileHeight;
			}
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


			potNewX = playerPos.x + playerPos.velX * fElapsedTime * 1.5f;
			potNewY = playerPos.y + playerPos.velY * fElapsedTime * 1.5f;
		

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

			else if(levelTiles[yTile * levelWidth + xTile] == 'a' || levelTiles[yTileOffset * levelWidth + xTile] == 'a')
			{
				//potNewX = xTile == 0 ? tileWidth : xTile * tileWidth + ((int)potNewX % (xTile * tileWidth)) + 1;
				playerPos.velX = 0.0f;
				playerPos.velY = 0.0f;
				gameOver = true;
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

			else if(levelTiles[yTile * levelWidth + xTileOffset] == 'a' || levelTiles[yTileOffset * levelWidth + xTileOffset] == 'a')
			{
				//potNewX = ((int)potNewX / tileWidth) * tileWidth;
				playerPos.velX = 0.0f;
				playerPos.velY = 0.0f;
				gameOver = true;
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

			else if(levelTiles[yTile * levelWidth + xTile] == 'a' || levelTiles[yTile * levelWidth + xTileOffset] == 'a')
			{
				//potNewY = (int)(yTile * tileHeight + tileHeight);
				playerPos.velX = 0.0f;
				playerPos.velY = 0.0f;
				gameOver = true;
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

			else if(levelTiles[yTileOffset * levelWidth + xTile] == 'a' || levelTiles[yTileOffset * levelWidth + xTileOffset] == 'a')
			{
				potNewY = ((int)potNewY / tileHeight) * tileHeight + (tileHeight / 20.0f) * playerPos.velY;
				playerPos.velY = 0.0f;
				playerPos.velX = 0.0f;
				gameOver = true;
			}
			for(int i = 0; i < noEnemys; i++)
			{
				if((potNewX + tileWidth) > enemyPos[i]->x && potNewX < (enemyPos[i]->x + tileWidth))
				{
					if((potNewY + tileHeight) > enemyPos[i]->y && potNewY < (enemyPos[i]->y + tileHeight))
					{
						playerPos.velY = 0.0f;
						playerPos.velX = 0.0f;
						gameOver = true;
					}
				}
			}
		}

		playerPos.x = potNewX;
		playerPos.y = potNewY;
	}


		if((playerPos.y + tileHeight) > ScreenHeight())
		{
			playerPos.y = ScreenHeight() - tileHeight;
			playerPos.velX = 0.0f;
			playerPos.velY = 0.0f;
		}
		

		//Enemy movement
		for(int i = 0; i < noEnemys; i++)
		{
			if(enemyPos[i]->spriteFrameCounter < 3.0f) enemyPos[i]->velX = -10.0f;
			else enemyPos[i]->velX = 10.0f;
			enemyPos[i]->spriteFrameCounter += fElapsedTime;
			enemyPos[i]->spriteFrameCounter = enemyPos[i]->spriteFrameCounter > 6.0f ? enemyPos[i]->spriteFrameCounter - 6.0f : enemyPos[i]->spriteFrameCounter;
			enemyPos[i]->x += enemyPos[i]->velX * fElapsedTime;
		}

		//std::cout << playerPos.x << ", " << playerPos.y << std::endl;

		//Panning
		cameraX = playerPos.x / tileWidth;

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
		if(!gameOver)
		{
			for(int x = -1; x <= FoVwidth; x++)
			{
				for(int y = 0; y < levelHeight; y++)
				{
					switch(levelTiles[y * levelWidth + x + xOffsetCam])
					{
						case '#' :
							DrawSprite(x * tileWidth - tileOffsetX, y * tileHeight, levelSprites[0].get());
							break;

						case 'a' :
							FillRect(x * tileWidth - tileOffsetX, y * tileHeight, tileWidth, tileHeight, olc::CYAN);
							DrawSprite(x * tileWidth - tileOffsetX, y * tileHeight, levelSprites[1].get());

							break;

						case '.' :
							FillRect(x * tileWidth - tileOffsetX, y * tileHeight, tileWidth, tileHeight, olc::CYAN);
							break;

	/*
						case 'e' :
							FillRect(x * tileWidth - tileOffsetX, y * tileHeight, tileWidth, tileHeight, olc::CYAN);
							DrawSprite(x * tileWidth - tileOffsetX - 1, y * tileHeight, enemySprites[0].get());
							break;
	*/
						default :
							break;
					}
				}
			}
		}

		for(int i = 0; i < noEnemys; i++)
		{
			if((enemyPos[i]->x > (xOffsetCam * tileWidth)) && (enemyPos[i]->x < ((xOffsetCam + FoVwidth) * tileWidth)))
			{
				FillRect(enemyPos[i]->x - (tileWidth * xOffsetCam) - tileOffsetX, enemyPos[i]->y, tileWidth * 2, tileHeight, olc::CYAN);
				DrawSprite(enemyPos[i]->x - (tileWidth * xOffsetCam) - tileOffsetX, enemyPos[i]->y, enemySprites[0].get());
			}
		}
		DrawSprite(playerPos.x - (tileWidth * xOffsetCam) - tileOffsetX, (playerPos.y), playerPos.sprites[playerPos.spriteNo].get());

		if(gameOver)
		{
			reset_sequence = true;
			int colorState = (int)(deadCounter / 0.25f);
			reset_game(colorState);
			deadCounter += fElapsedTime;
			if(deadCounter > 4.0f) reset_sequence = false;

			if(!reset_sequence)
			{
				playerPos.x = tileWidth + 2;
				playerPos.y = 100.0f;
				deadCounter = 0.0f;
				gameOver = false;
			}
			
		}

		

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