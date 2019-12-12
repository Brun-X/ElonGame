
/*

IMPLEMNTED WITH THE OLCPIXELGAMEENGINE FROM:
		    
		    www.onelonecoder.com

		  GREAT THANKS TO JAVIDX9

*/


#define OLC_PGE_APPLICATION
#define __LINUX__

#include "olcPixelGameEngine.h"
#include <cmath>
#include <vector>


typedef std::pair <float, float> fPair;
typedef std::pair <int, int> iPair;


struct Ball
{
	float px;
	float py;
	float vx;
	float vy;
	float radius;
};

class Pico : public olc::PixelGameEngine
{
private:
	int levelWidth;
	int levelHeight;
	std::string levelTiles;
	int tileWidth = 8;
	int tileHeight = 8;

	std::vector<fPair> modelCircle;
	std::vector<Ball*> balls;
	int circlePoints = 20;
	float scaleFactor = 10.0;

	std::vector< std::vector<iPair> > lines;

public:
	Pico()
	{
		sAppName = "Pico";
	}

	bool OnUserCreate() override
	{

		modelCircle.push_back({ 0.0f, 0.0f });
		
		for(int i = 0; i < circlePoints; i++)
		{
			modelCircle.push_back({ cosf(i / (float)(circlePoints -1) * 2.00f * 3.14159f) , sinf(i / (float)(circlePoints - 1) * 2.0f * 3.14159f) });
		}

		Ball* b1 = new Ball{100.0, 100.0, 0.0, 0.0, 50.0};
		balls.push_back(b1);

		

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

		for(int x = 0; x < ScreenWidth(); x++)
		{
			for(int y = 0; y < ScreenHeight(); y++)
			{
				Draw(x, y, olc::BLUE);
			}
		}

		for(auto ball : balls)
		{
			for(int s = 0; s < circlePoints; s++)
			{
				float lastX, lastY;
				float pointX = (modelCircle[s].first * ball->radius) + ball->px;
				float pointY = (modelCircle[s].second * ball->radius) + ball->py;

				if(s > 0)
				{
					//if(s <= circlePoints / 2)
						makeLine(lastX, lastY, pointX, pointY, olc::BLACK);
					//else makeLine(pointX, pointY, lastX, lastY, olc::BLACK);
				}
				lastX = pointX;
				lastY = pointY;

				Draw(pointX, pointY, olc::WHITE);
			}
		}

		SetPixelMode(olc::Pixel::NORMAL);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//User input
/*
		if(GetKey(olc::Key::UP).bHeld) balls[0]->py -= 2;
		if(GetKey(olc::Key::DOWN).bHeld) balls[0]->py += 2;
		if(GetKey(olc::Key::RIGHT).bHeld) balls[0]->px += 2;
		if(GetKey(olc::Key::LEFT).bHeld) balls[0]->px -= 2;		
*/		
		//Collision detection


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

/*
		
		for(int x = 0; x < levelWidth; x++)
		{
			for(int y = 0; y < levelHeight; y++)
			{
				olc::Pixel p = levelTiles[y * levelWidth + x] == '#' ? olc::DARK_RED : olc::BLUE;
				FillRect(x * tileWidth, y * tileHeight, tileWidth, tileHeight, p);
			}
		}
*/

		

		for(auto vec : lines)
		{
			for(auto l : vec)
			{
				Draw(l.first, l.second, olc::BLACK);
			}
		}

		return true;
	}

	bool makeLine(float x1, float y1, float x2, float y2, olc::Pixel p)
	{
		std::vector<iPair> line;

		float dx, dy;

		dx = x2 - x1;
		dy = y2 - y1;

		if(dx == 0 || dy == 0) return false;

		float gradientX = dx / dy;
		float gradientY = dy / dx;

		float tmp = 0.000;

		if(dx < 0 && dy < 0)
		{
			for(int cx = 0; cx <= (int)fabs(dx); cx++)
			{
				tmp -= fabs(gradientY);
				line.push_back({x1 - cx, y1 + (int)tmp});
			}
		}

		else if(dx > 0 && dy < 0)
		{
			for(int cx = 0; cx <= (int)dx; cx++)
			{
				tmp -= fabs(gradientY);
				line.push_back({x1 + cx, y1 + (int)tmp});
			}
		}

		else if(dx > 0 && dy > 0)
		{
			for(int cx = 0; cx <= (int)dx; cx++)
			{
				tmp += fabs(gradientY);
				line.push_back({x1 + cx, y1 + (int)tmp});
			}
		}

		else if(dx < 0 && dy > 0)
		{
			for(int cx = 0; cx <= (int)fabs(dx); cx++)
			{
				tmp += fabs(gradientY);
				line.push_back({x1 - cx, y1 + (int)tmp});
			}
		}
		lines.push_back(line);
		return true;
	}

};



int main()
{
	Pico demo;
	if(demo.Construct(256, 240, 4, 4)) demo.Start();


	return 0;
}