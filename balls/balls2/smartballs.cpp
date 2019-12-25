#define OLC_PGE_APPLICATION
#define __LINUX__

//#include "olcPixelGameEngine.h"
#include "Objects.hpp"
#include <cmath>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <cstdlib>
#include <ctime>

class Pico : public olc::PixelGameEngine
{
private:
	std::vector<iPair> grid_;
	int gridSize_ = 32;
	unsigned int shapeId = 0;
	std::vector< boost::shared_ptr<Object> > objects;
	iPair newMousePos;
	iPair lastMousePos;
	iPair movingLastMousePos;
	bool drawing_ = false;
	boost::shared_ptr<Object> drawingObject;
	//Object** selectedObject_ = nullptr;
	boost::shared_ptr<Object> selectedObject_;
	int selectedType_ = NONE;
	int boxcorner_ = NONE;

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


		objects.push_back(boost::shared_ptr<Object> (new Box(300.0f, 300.0f, 370.0f, 370.0f)));

/*
		for(int i = 0; i < 60; i++)
		{
			objects.push_back(boost::shared_ptr<Object> (new Ball(((rand() + 10) % (ScreenWidth() - 10)), ((rand() + 10) % (ScreenHeight() - 10)), 0.0, 0.0, (float)((1 + (rand() % 5)) * 8))));
			
		}
*/	

		SetPixelMode(olc::Pixel::NORMAL);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//User input
		

		//if(GetKey(olc::Key::UP).bHeld) static_cast<Ball*>(objects[0])->moveDirection(1.0f);
		//if(GetKey(olc::Key::DOWN).bHeld) static_cast<Ball*>(objects[0])->moveDirection(-1.0f);
		//if(GetKey(olc::Key::RIGHT).bHeld) static_cast<Ball*>(objects[0])->rotation_ += 1.0f * fElapsedTime;
		//if(GetKey(olc::Key::LEFT).bHeld) static_cast<Ball*>(objects[0])->rotation_ -= 1.0f * fElapsedTime;		

		
		if(GetMouse(0).bPressed) 
		{
			lastMousePos = {GetMouseX(), GetMouseY()};
			movingLastMousePos = {GetMouseX(), GetMouseY()};
			
/*
			if(GetKey(olc::Key::B).bHeld) 
			{
				shapeId = BOX;
				objects.push_back(new Box(lastMousePos.first, lastMousePos.second, lastMousePos.first + 1, lastMousePos.second + 1));
				selectedBox_ = static_cast<Box*>(objects.back());
			}*/
			//else if(GetKey(olc::Key::C).bHeld) shapeId = BALL;
			if(GetKey(olc::Key::R).bHeld) shapeId = RESIZE;
			else shapeId = MOVE;
			//if(shapeId != MOVE) drawing_ = true;
			//selectedType_ = NONE;

			if(shapeId == MOVE || shapeId == RESIZE)
			{
				for(auto &o : objects)
				{	
					if(o->shape_ == BALL)
					{
						//Ball* b = static_cast<Ball*>(o);
						if(boost::static_pointer_cast<Ball>(o)->isPointInsideObject(lastMousePos))
						{
							selectedObject_ = o;
							selectedType_ = BALL;
							break;
						}
					}

					else if(o->shape_ == BOX)
					{
						if(boost::static_pointer_cast<Box>(o)->isPointInsideObject(&boxcorner_, lastMousePos))
						{
							//std::cout << "selectedObject_ got box ptr" << std::endl;
							selectedObject_ = o;
							selectedType_ = BOX;
							break;
						}
					}
					
				}
			}
		}

		if(GetMouse(0).bHeld) 
		{
			//std::cout << selectedObject_ << std::endl;
			newMousePos = {GetMouseX(), GetMouseY()};
			if(shapeId == MOVE)
			{
				if((selectedType_ == BALL) && (selectedObject_ != nullptr))
				{
					selectedObject_->px_ = newMousePos.first;
					selectedObject_->py_ = newMousePos.second;
				
					selectedObject_->vx_ += (newMousePos.first - movingLastMousePos.first);
					selectedObject_->vy_ += (newMousePos.second - movingLastMousePos.second);
				
					movingLastMousePos = newMousePos;
				}

				else if((selectedType_ == BOX) && (selectedObject_ != nullptr))
				{
					(selectedObject_)->px_ = newMousePos.first;
					(selectedObject_)->py_ = newMousePos.second;
				
					//selectedObject_->vx_ += (newMousePos.first - movingLastMousePos.first);
					//selectedObject_->vy_ += (newMousePos.second - movingLastMousePos.second);
				
					movingLastMousePos = newMousePos;
				}
			}
			else if(shapeId == RESIZE)
			{
				if(selectedObject_ != nullptr && selectedType_ == BOX)
				{
					int dummy;
					//selectedBox_->isPointInsideObject(&dummy, newMousePos);
					boost::static_pointer_cast<Box>(selectedObject_)->resizeObject(boxcorner_, newMousePos);
				} 
				//std::cout << boxcorner << std::endl;
			}
		}

		if(GetMouse(0).bReleased)
		{
			
			//if(shapeId == BOX) objects.push_back(new Box(lastMousePos.first, lastMousePos.second, newMousePos.first, newMousePos.second));
			
			//else if(shapeId == BALL) objects.push_back(new Ball((float)lastMousePos.first, (float) lastMousePos.second, (float) newMousePos.first, (float) newMousePos.second, -1.0f, 0.0f));

			//else if(shapeId == LINE) objects.push_back(new Line(lastMousePos.first, lastMousePos.second, newMousePos.first, newMousePos.second));



			if(shapeId == RESIZE)
			{
				if(selectedType_ == BOX)
				{
					//boost::static_pointer_cast<Box>(selectedObject_)->setCenterAfterResize()r;
				}

			}
//			drawing_ = false;
//			drawingObject.reset();
			selectedObject_.reset();
			shapeId = MOVE;
			boxcorner_ = NONE;
			selectedType_ = NONE;
			//std::cout << "selectedType_: " << selectedObject_ << std::endl;
		}


		//Resolve physics
		
		for(auto b : objects)
		{

					
					srand(time(0));
					float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

					b->rotation_ = r * fElapsedTime * 0.1f;

					b->vx_ = cosf(b->rotation_);
					b->vy_ = sinf(b->rotation_);

					//b->px_ += b->vx_ * b->velocity_ * fElapsedTime * (r + 0.6f);
					//b->py_ += b->vy_ * b->velocity_ * fElapsedTime * (r + 0.6f);


					b->py_ = b->py_ > ScreenHeight() ? 0.0f : b->py_;
					//b->vy_ = b->vy_ > ScreenHeight() ? 0.0f : b->vy_;
					b->px_ = b->px_ > ScreenWidth() ? 0.0f : b->px_;
					//b->vx_ = b->vx_ > ScreenWidth() ? 0.0f : b->vx_;

					b->py_ = b->py_ >= 0.0f ? b->py_ : ScreenHeight();
					//b->vy_ = b->vy_ >= 0.0f ? b->vy_ : ScreenHeight();
					b->px_ = b->px_ >= 0.0f ? b->px_ : ScreenWidth();
					//b->vx_ = b->vx_ >= 0.0f ? b->vx_ : ScreenWidth();
		}

		


		for(auto &b : objects)
		{
			if(b->shape_ == BALL)
			{
				for(auto a : objects)
				{
					if(b != a)
					{						
						if(a->shape_ == BALL)
							{
								//Ball* b = static_cast<Ball*>(o);
								//Ball* a = static_cast<Ball*>(oNested);
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