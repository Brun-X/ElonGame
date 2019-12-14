
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

int circlePoints = 45;

class Object
{
public:
	std::vector<std::vector<iPair>> objectLines_;
	
	virtual ~Object() {}

	virtual void calculateShape() {}

	static bool makeLine(float x1, float y1, float x2, float y2, std::vector<std::vector<iPair>>* v)
	{
		std::vector<iPair> line;
		float gradientX, gradientY;
		float dx, dy;

		dx = x2 - x1;
		dy = y2 - y1;

		if(dx == 0 && dy == 0) return false;

		gradientX = dx != 0.0f ? dx / dy : 0.0f;
		gradientY = dy != 0.0f ? dy / dx : 0.0f;

		float tmp = 0.000;
		float round = 0.8;

		if(fabs(gradientX) >= fabs(gradientY))
		{

			if(dx < 0 && dy < 0)
			{
				for(int cx = 0; cx <= (int)fabs(dx); cx++)
				{
					tmp -= fabs(gradientY);
					tmp = (abs(tmp) - (int)abs(tmp)) < round ? tmp : (int)tmp - 1;
					line.push_back({x1 - cx, y1 + (int)tmp});
				}
			}

			else if(dx > 0 && dy < 0)
			{
				for(int cx = 0; cx <= (int)dx; cx++)
				{
					tmp -= fabs(gradientY);
					tmp = (abs(tmp) - (int)abs(tmp)) < round ? tmp : (int)tmp - 1;
					line.push_back({x1 + cx, y1 + (int)tmp});
				}
			}

			else if(dx > 0 && dy > 0)
			{
				for(int cx = 0; cx <= (int)dx; cx++)
				{
					tmp += fabs(gradientY);
					tmp = (abs(tmp) - (int)abs(tmp)) < round ? tmp : (int)tmp + 1;
					line.push_back({x1 + cx, y1 + (int)tmp});
				}
			}

			else if(dx < 0 && dy > 0)
			{
				for(int cx = 0; cx <= (int)abs(dx); cx++)
				{
					tmp += fabs(gradientY);
					tmp = (abs(tmp) - (int)fabs(tmp)) < round ? tmp : (int)tmp + 1;
					line.push_back({x1 - cx, y1 + (int)tmp});
				}
			}
		}

		if(fabs(gradientY) > fabs(gradientX))
		{

			if(dx < 0 && dy < 0)
			{
				for(int cy = 0; cy <= (int)fabs(dy); cy++)
				{
					tmp -= fabs(gradientX);
					tmp = (abs(tmp) - (int)abs(tmp)) < round ? tmp : (int)tmp - 1;
					line.push_back({x1 + (int)tmp, y1 - cy});
				}
			}

			else if(dx > 0 && dy < 0)
			{
				for(int cy = 0; cy <= (int)fabs(dy); cy++)
				{
					tmp += fabs(gradientX);
					tmp = (abs(tmp) - (int)abs(tmp)) < round ? tmp : (int)tmp + 1;
					line.push_back({x1 + (int)tmp, y1 - cy});
				}
			}

			else if(dx > 0 && dy > 0)
			{
				for(int cy = 0; cy <= (int)fabs(dy); cy++)
				{
					tmp += fabs(gradientX);
					tmp = (abs(tmp) - (int)abs(tmp)) < round ? tmp : (int)tmp + 1;
					line.push_back({x1 + (int)tmp, y1 + cy});
				}
			}

			else if(dx < 0 && dy > 0)
			{
				for(int cy = 0; cy <= (int)fabs(dy); cy++)
				{
					tmp -= fabs(gradientX);
					tmp = (abs(tmp) - (int)abs(tmp)) < round ? tmp : (int)tmp - 1;
					line.push_back({x1 + (int)tmp, y1 + cy});
				}
			}
		}
	
		if(dx == 0.0f)
		{
			for(int cy = 0; cy <= (int)fabs(dy); cy++)
			{
				float ySign = dy < 0.0f ? -1.0f : 1.0f;
				line.push_back({x1, y1 + (ySign * cy)});
			}
		}

		else if(dy == 0.0f)
		{
			for(int cx = 0; cx <= (int)fabs(dx); cx++)
			{
				float xSign = dx < 0.0f ? -1.0f : 1.0f;
				line.push_back({x1 + xSign * cx, y1});
			}
		}

		v->push_back(line);
		return true;
	}

};

class Line : public Object
{
public:
	Line() : Object() {};

	Line(float x1, float y1, float x2, float y2) : x1_(x1), y1_(y1), x2_(x2), y2_(x2)
	{
		makeLine(x1_, y1_, x2_, y2_, &objectLines_);
	} 

public:
	float x1_, y1_, x2_, y2_;
	std::vector<iPair> line_;
};

class Ball : public Object
{
public:
	Ball() : Object() {}

	Ball(int id, float px, float py, float vx, float vy, float radius, float rotation) : id_(id), px_(px), py_(py), vx_(vx), vy_(vy), radius_(radius), rotation_(rotation)
	{
		mc_.push_back({ 0.0f, 0.0f });
		
		for(int i = 0; i <= circlePoints; i++)
		{
			mc_.push_back({ cosf(i / (float)(circlePoints -1) * 2.00f * 3.14159f) , sinf(i / (float)(circlePoints - 1) * 2.0f * 3.14159f) });
		}
	}

	virtual void calculateShape() override 
	{
		float pointX, pointY;
		float lastX = mc_[1].first * radius_ + px_;
		float lastY = mc_[1].second * radius_ + py_;
		objectLines_.clear();

		for(int s = 0; s < circlePoints + 1; s++)
		{
			if(s > 1) 
			{
				pointX = (mc_[s + 1].first * radius_) + px_;
				pointY = (mc_[s + 1].second * radius_) + py_;
				
				makeLine(lastX, lastY, pointX, pointY, &objectLines_);
				
				pointX = s < circlePoints + 1 ? pointX : mc_[1].first * radius_ + px_;
				pointY = s < circlePoints + 1 ? pointY : mc_[1].second * radius_ + py_;
				lastX = pointX;
				lastY = pointY;
			}
			else
			{
				float phi = rotation_ * (M_PI / 180);
				vx_ = cosf(phi) * radius_ + px_;
				vy_ = -sinf(phi) * radius_ + py_;

				makeLine(px_, py_, vx_, vy_, &objectLines_);
			} 	
		}
	}


	void moveDirection(float stepSize)
	{
		float phi = rotation_ * (M_PI / 180);
		px_ = cosf(phi) * stepSize + px_;
		py_ = -sinf(phi) * stepSize + py_;
	}



public:
	int id_;
	float px_;
	float py_;
	float vx_;
	float vy_;
	float radius_;
	float rotation_ = 0;

private:
	std::vector<fPair> mc_;
	
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
	std::vector<Object*> objects;

	//std::vector<std::vector<iPair>> underConstruction_ = nullptr;

public:
	Pico()
	{
		sAppName = "Pico";
	}

	bool OnUserCreate() override
	{

		/*
		modelCircle.push_back({ 0.0f, 0.0f });
		
		for(int i = 0; i <= circlePoints; i++)
		{
			modelCircle.push_back({ cosf(i / (float)(circlePoints -1) * 2.00f * 3.14159f) , sinf(i / (float)(circlePoints - 1) * 2.0f * 3.14159f) });
		}
		*/
		
		for(int i = 0; i < 3; i++)
		{
			objects.push_back( new Ball(i, ((rand() + 10) % (ScreenWidth() - 10)), ((rand() + 10) % (ScreenHeight() - 10)), 0.0, 0.0, (float)((1 + (rand() % 5)) * 8), 0.0f));
			
		}
	

		

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

		/*
		for(auto ball : balls)
		{
			for(int s = 0; s <= circlePoints; s++)
			{
				float pointX = (modelCircle[s].first * ball->radius_) + ball->px_;
				float pointY = (modelCircle[s].second * ball->radius_) + ball->py_;

				Draw(pointX, pointY, olc::WHITE);
			}
		}*/

		SetPixelMode(olc::Pixel::NORMAL);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//User input



		if(GetKey(olc::Key::UP).bHeld) static_cast<Ball*>(objects[0])->moveDirection(1.0f);
		if(GetKey(olc::Key::DOWN).bHeld) static_cast<Ball*>(objects[0])->moveDirection(-1.0f);
		if(GetKey(olc::Key::RIGHT).bHeld) static_cast<Ball*>(objects[0])->rotation_ += 1.0f;
		if(GetKey(olc::Key::LEFT).bHeld) static_cast<Ball*>(objects[0])->rotation_ -= 1.0f;		


/*
		iPair lastMousePos = 

		if(GetMouse(0).bPressed) 
		{

			underConstruction_.push_back(new Line(GetMouseX(), GetMouseY(), GetMouseX(), GetMouseY()));
		}

		if(GetMouse(0).bHeld) 
		{}

*/

		//Collision detection

		for(auto o : objects)
		{
			o->calculateShape();
		}

		for(int x = 0; x < ScreenWidth(); x++)
		{
			for(int y = 0; y < ScreenHeight(); y++)
			{
				Draw(x, y, olc::BLUE);
			}
		}

		for(auto obj : objects)
		{
			for(auto lines : obj->objectLines_)
			{
				for(auto line : lines)
				Draw(line.first, line.second, olc::BLACK);
			}
		}

		return true;
	}

	

};



int main()
{
	Pico demo;
	if(demo.Construct(256, 240, 4, 4)) demo.Start();


	return 0;
}