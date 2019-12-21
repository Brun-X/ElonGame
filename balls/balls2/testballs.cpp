#define OLC_PGE_APPLICATION
#define __LINUX__

//#include "olcPixelGameEngine.h"
#include "Objects.hpp"
#include <cmath>
#include <vector>
#include <boost/shared_ptr.hpp>

class Pico : public olc::PixelGameEngine
{
private:
	int levelWidth;
	int levelHeight;
	std::string levelTiles;
	int tileWidth = 8;
	int tileHeight = 8;

	std::vector<iPair> grid_;
	int gridSize_ = 32;
	unsigned int shapeId = 0;
	std::vector<Object*> objects;
	iPair newMousePos;
	iPair lastMousePos;
	iPair movingLastMousePos;
	bool drawing_ = false;
	boost::shared_ptr<Object> drawingObject;
	Ball* selectedBall_ = nullptr;

public:
	Pico()
	{
		sAppName = "Pico";
	}

	bool OnUserCreate() override
	{
		for(int x = 0; x < ScreenWidth() / gridSize_; x++)
		{
			for(int y = 0; y < ScreenHeight() / gridSize_; y++)
			{
				grid_.push_back({x * gridSize_, y * gridSize_});
			}
		}


		objects.push_back(new Box(20.0f, 20.0f, 150.0f, 150.0f));


		
		for(int i = 0; i < 3; i++)
		{
			objects.push_back( new Ball(((rand() + 10) % (ScreenWidth() - 10)), ((rand() + 10) % (ScreenHeight() - 10)), 0.0, 0.0, (float)((1 + (rand() % 5)) * 8)));
			
		}
	

		SetPixelMode(olc::Pixel::NORMAL);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//User input
/*
		if(GetKey(olc::Key::UP).bHeld) static_cast<Ball*>(objects[0])->moveDirection(1.0f);
		if(GetKey(olc::Key::DOWN).bHeld) static_cast<Ball*>(objects[0])->moveDirection(-1.0f);
		if(GetKey(olc::Key::RIGHT).bHeld) static_cast<Ball*>(objects[0])->rotation_ += 1.0f;
		if(GetKey(olc::Key::LEFT).bHeld) static_cast<Ball*>(objects[0])->rotation_ -= 1.0f;		
*/

		//Resolve physics
		for(auto o : objects)
		{
			if(o->shape_ == BALL)
			{
				Ball* b = static_cast<Ball*>(o);
				if(b != selectedBall_)
				{
					b->moveDirection(b->velocity_);

					b->py_ = b->py_ > ScreenHeight() ? 0.0f : b->py_;
					b->vy_ = b->vy_ > ScreenHeight() ? 0.0f : b->vy_;
					b->px_ = b->px_ > ScreenWidth() ? 0.0f : b->px_;
					b->vx_ = b->vx_ > ScreenWidth() ? 0.0f : b->vx_;

					b->py_ = b->py_ >= 0.0f ? b->py_ : ScreenHeight();
					b->vy_ = b->vy_ >= 0.0f ? b->vy_ : ScreenHeight();
					b->px_ = b->px_ >= 0.0f ? b->px_ : ScreenWidth();
					b->vx_ = b->vx_ >= 0.0f ? b->vx_ : ScreenWidth();
				}
			}
		}
		
		//Drawing screen

		for(int x = 0; x < ScreenWidth(); x++)
		{
			for(int y = 0; y < ScreenHeight(); y++)
			{
				Draw(x, y, olc::BLUE);
			}
		}

		for(auto dots : grid_)
		{
			Draw(dots.first, dots.second);
		}

		for(auto obj : objects)
		{
			obj->drawShape(this);
		}

		
		return true;
	}
};



int main()
{
	Pico demo;
	if(demo.Construct(800, 600, 2, 2)) demo.Start();


	return 0;
}