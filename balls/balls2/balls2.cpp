
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
#include <boost/shared_ptr.hpp>

enum { MOVE=0, BALL=1, BOX=2, LINE=3 };


typedef std::pair <float, float> fPair;
typedef std::pair <int, int> iPair;

int circlePoints = 45;

class Object
{
public:
	std::vector<std::vector<iPair>> objectLines_;
	int id_;
	int shape_ = 0;
	
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

	Line(float x1, float y1, float x2, float y2) : x1_(x1), y1_(y1), x2_(x2), y2_(y2)
	{
		makeLine(x1_, y1_, x2_, y2_, &objectLines_);
		shape_ = LINE;
	}

	virtual void calculateShape() override {}

public:
	float x1_, y1_, x2_, y2_;
	std::vector<iPair> line_;
};

class Box : public Object
{
public:
	float x1_;
	float y1_;
	float x2_;
	float y2_;

public:
	Box() : Object() {};

	Box(float x1, float y1, float x2, float y2) : x1_(x1), y1_(y1), x2_(x2), y2_(y2)
	{
		makeLine(x1_, y1_, x2_, y1_, &objectLines_);
		makeLine(x2_, y1_, x2_, y2_, &objectLines_);
		makeLine(x2_, y2_, x1_, y2_, &objectLines_);
		makeLine(x1_, y2_, x1_, y1_, &objectLines_);
		shape_ = BOX;
	}

	virtual void calculateShape() override {}
};

class Ball : public Object
{
public:
	Ball() : Object() {}

	Ball(float px, float py, float vx, float vy, float radius, float rotation) : px_(px), py_(py), vx_(vx), vy_(vy), radius_(radius), rotation_(rotation)
	{
		shape_ = BALL;
		mc_.push_back({ 0.0f, 0.0f });
		
		for(int i = 0; i <= circlePoints; i++)
		{
			mc_.push_back({ cosf(i / (float)(circlePoints -1) * 2.00f * 3.14159f) , sinf(i / (float)(circlePoints - 1) * 2.0f * 3.14159f) });
		}

		if(radius < 0)
		{
			radius_ = sqrt((fabs(px - vx) * fabs(px - vx)) + (fabs(py - vy) * fabs(py - vy)));
			radius_ = px_ < vx_ ? radius_ : -radius_;

			rotation_ = acosf(fabs((vx_ - px_)) / radius_);
			if(vy_ > py_ && radius_ > 0.0f) rotation_ = (2 * M_PI) - rotation_;
			else if(vy_ > py_ && radius_ < 0.0f) rotation_ = -rotation_;
		}

		float pointX, pointY;
		float lastX = mc_[1].first * radius_ + px_;
		float lastY = mc_[1].second * radius_ + py_;

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
				makeLine(px_, py_, vx_, vy_, &objectLines_);
			} 	
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
				makeLine(px_, py_, vx_, vy_, &objectLines_);
			} 	
		}
	}


	void moveDirection(float stepSize)
	{
/*
		rotation_ = acosf(fabs((vx_ - px_)) / radius_);
		if(vy_ > py_ && radius_ > 0.0f) rotation_ = (2 * M_PI) - rotation_;
		else if(vy_ > py_ && radius_ < 0.0f) rotation_ = -rotation_;
*/

		float directionX = cosf(rotation_) * stepSize;
		float directionY = -sinf(rotation_) * stepSize;

		px_ += directionX;
		py_ += directionY;
		vx_ += directionX;
		vy_ += directionY;
	}

	bool isPointInsideCircle(float x, float y)
	{
		return sqrtf(((px_ - x) * (px_ - x)) + ((py_ - y) * (py_ - y))) < fabs(radius_) ? true : false;
	}



public:
	float px_;
	float py_;
	float vx_;
	float vy_;
	float radius_;
	float rotation_;
	float velocity_ = 0.0f;
	float acceleration_ = 1.0f;

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


		//objects.push_back(new Box(20.0f, 20.0f, 150.0f, 150.0f));


		/*
		for(int i = 0; i < 3; i++)
		{
			objects.push_back( new Ball(((rand() + 10) % (ScreenWidth() - 10)), ((rand() + 10) % (ScreenHeight() - 10)), 0.0, 0.0, (float)((1 + (rand() % 5)) * 8), 0.0f));
			
		}
	*/

		

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

		if(GetMouse(0).bPressed) 
		{
			lastMousePos = {GetMouseX(), GetMouseY()};
			movingLastMousePos = {GetMouseX(), GetMouseY()};
			drawing_ = true;

			if(GetKey(olc::Key::B).bHeld) shapeId = BOX;
			else if(GetKey(olc::Key::C).bHeld) shapeId = BALL;
			else if(GetKey(olc::Key::L).bHeld) shapeId = LINE;
			else shapeId = MOVE;

			if(shapeId == MOVE)
			{
				for(auto o : objects)
				{	
					if(o->shape_ == BALL)
					{
						//Ball* b = static_cast<Ball*>(o);
						if(static_cast<Ball*>(o)->isPointInsideCircle(lastMousePos.first, lastMousePos.second))
						{

							selectedBall_ = static_cast<Ball*>(o);
							
							break;
						}
					}
					
				}
			}
		}

		if(GetMouse(0).bHeld) 
		{
			newMousePos = {GetMouseX(), GetMouseY()};
			if(selectedBall_ != nullptr)
			{
				selectedBall_->px_ = newMousePos.first;
				selectedBall_->py_ = newMousePos.second;
			
				selectedBall_->vx_ += (newMousePos.first - movingLastMousePos.first);
				selectedBall_->vy_ += (newMousePos.second - movingLastMousePos.second);
			
				movingLastMousePos = newMousePos;
			}
		}

		if(GetMouse(0).bReleased)
		{
			if(shapeId == BOX) objects.push_back(new Box(lastMousePos.first, lastMousePos.second, newMousePos.first, newMousePos.second));
			
			else if(shapeId == BALL) objects.push_back(new Ball((float)lastMousePos.first, (float) lastMousePos.second, (float) newMousePos.first, (float) newMousePos.second, -1.0f, 0.0f));

			else if(shapeId == LINE) objects.push_back(new Line(lastMousePos.first, lastMousePos.second, newMousePos.first, newMousePos.second));

			if(shapeId == MOVE) selectedBall_ = nullptr;

			drawing_ = false;
			drawingObject.reset();
			shapeId = MOVE;
		}


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

		//Collision detection

		std::vector<std::pair<Ball*,Ball*>> ballsCollided;

		for(auto o : objects)
		{
			if(o->shape_ == BALL)
			{
				for(auto oNested : objects)
				{
					if(o != oNested)
					{						
						//for(auto lines : oNested->objectLines_)
						if(oNested->shape_ == BALL)
						
							//for(auto line : lines)
							{
								Ball* b = static_cast<Ball*>(o);
								Ball* a = static_cast<Ball*>(oNested);
								float hyp = sqrtf( (a->px_ - b->px_) * (a->px_ - b->px_) + (a->py_ - b->py_) * (a->py_ - b->py_) );
								if(hyp < (fabs(a->radius_) + fabs(b->radius_)))
								{
									

									float overlap = 0.5f * (hyp - a->radius_ - b->radius_);

									a->px_ -= overlap * (a->px_ - b->px_) / hyp;
									a->py_ -= overlap * (a->py_ - b->py_) / hyp;

									a->vx_ -= overlap * (a->px_ - b->px_) / hyp;
									a->vy_ -= overlap * (a->py_ - b->py_) / hyp;

									b->px_ += overlap * (a->px_ - b->px_) / hyp;
									b->py_ += overlap * (a->py_ - b->py_) / hyp;

									b->vx_ += overlap * (a->px_ - b->px_) / hyp;
									b->vy_ += overlap * (a->py_ - b->py_) / hyp;

									//ballsCollided.push_back({a,b});

								}
							}
									
					}
				}
			}
		}
/*
		//Dynamic collision
		for(auto col : ballsCollided)
		{
			float nx = (b->px_ - a->px_) / hyp;
			float ny = (b->py_ - a->py_) / hyp;

			float tx = -ny;
			float ty = nx;

			dpTan = ballsCollided.first-> * tx + line.second * ty;
		}
*/
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
			obj->calculateShape();
			for(auto lines : obj->objectLines_)
			{
				for(auto line : lines)
				Draw(line.first, line.second, olc::WHITE);
			}
		}

		if(drawing_)
		{
			switch(shapeId)
			{
				/*
				case MOVE :
				{
					selectedBall_->moveDirection( sqrtf( ((selectedBall_->px_ - newMousePos.first) * (selectedBall_->px_ - newMousePos.first)) + ((selectedBall_->py_ - newMousePos.second) * (selectedBall_->py_ - newMousePos.second))));
					std::cout << "test" << std::endl;
				}*/

				case BOX :
					{
					boost::shared_ptr<Object> b(new Box(lastMousePos.first, lastMousePos.second, newMousePos.first, newMousePos.second));
					drawingObject = boost::static_pointer_cast<Object>(b);
					break;
					}

				case BALL :
					{
					boost::shared_ptr<Object> c(new Ball((float)lastMousePos.first, (float) lastMousePos.second, (float) newMousePos.first, (float) newMousePos.second, -1.0f, 0.0f));
					drawingObject = boost::static_pointer_cast<Object>(c);
					break;
					}

				case LINE :
					{
					boost::shared_ptr<Object> l(new Line(lastMousePos.first, lastMousePos.second, newMousePos.first, newMousePos.second)); 
					drawingObject = boost::static_pointer_cast<Object>(l);
					break;
					}

				default :
					drawingObject.reset();
					break;
			}

			if(drawingObject != nullptr)
			{
				for(auto lines : drawingObject->objectLines_)
				{
					for(auto line: lines)
					{
						Draw(line.first, line.second, olc::RED);
					}
				}
			}
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