/*

IMPLEMNTED WITH THE OLCPIXELGAMEENGINE FROM:
		    
		    www.onelonecoder.com

		  GREAT THANKS TO JAVIDX9

*/

#define OLC_PGE_APPLICATION

#include <cmath>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "olcPixelGameEngine.h"

enum { MOVE=0, BALL=1, BOX=2, LINE=3, RESIZE=4, NONE=99 };

typedef std::pair <float, float> fPair;
typedef std::pair <int, int> iPair;

class Object : public olc::PixelGameEngine
{
public:
	std::vector<std::vector<iPair>> objectLines_;
	std::vector<fPair> objectModel_;
	int id_;
	int shape_ = 0;
	float px_;
	float py_;
	float vx_ = 0.0f;
	float vy_ = 0.0f;
	float radius_;
	float rotation_ = 0.0f;
	float velocity_ = 30.0f;
	float acceleration_ = 1.0f;
	olc::Pixel color_ = olc::WHITE;

protected:

	Object() {}

	Object(float px, float py, float vx = 0.0f, float vy = 0.0f, float radius = 1.0f) : px_(px), py_(py), vx_(vx), vy_(vy), radius_(radius) {}

public:

	virtual ~Object() {}

	virtual void calculateShape() {}

	virtual bool isPointInsideObject() {return false;}
	
	virtual void drawShape(olc::PixelGameEngine* engine) = 0;

	static bool LlobstidrawLine(olc::PixelGameEngine* engine, float x1, float y1, float x2, float y2, olc::Pixel color)
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
		float round = 0.5;

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

		for(int i = 0; i < line.size(); i++)
		{
			engine->Draw(line[i].first, line[i].second, color);
		}
		return true;
	}

	std::vector<fPair> translatePolygon(const std::vector<fPair> &vecObjectPoints, float x, float y, float rot = 0.0f, float scale = 1.0f)
	{
		static std::vector<fPair> vecTranslatedModel;
		int verts = vecObjectPoints.size();
		vecTranslatedModel.resize(verts);

		//rotate
		for(int i = 0; i < verts; i++)
		{
			vecTranslatedModel[i].first = vecObjectPoints[i].first * cosf(rot) - vecObjectPoints[i].second * sinf(rot);
			vecTranslatedModel[i].second = vecObjectPoints[i].first * sinf(rot) + vecObjectPoints[i].second * cosf(rot);
		}

		//Scale
		if(scale != 1.0f)
		{
			for(int i = 0; i < verts; i++)
			{
				vecTranslatedModel[i].first = vecTranslatedModel[i].first * scale;
				vecTranslatedModel[i].second = vecTranslatedModel[i].second * scale;
			}
		}

		//Move / translate
		for(int i = 0; i < verts; i++)
		{
			vecTranslatedModel[i].first = vecTranslatedModel[i].first + x;
			vecTranslatedModel[i].second = vecTranslatedModel[i].second + y;
		}
		
		return vecTranslatedModel;
	}

};

class Line : public Object
{
public:
	Line() : Object() {};

	Line(float x1, float y1, float x2, float y2)
	{
		objectModel_.push_back({x1, y1});
		objectModel_.push_back({x2, y2});
		shape_ = LINE;
	}

	virtual void calculateShape() override {}

	virtual void drawShape(olc::PixelGameEngine * engine) override
	{
		std::vector<fPair> t = translatePolygon(objectModel_, px_, py_, 0.0f, 1.0f);
		LlobstidrawLine(engine, t[0].first, t[0].second, t[1].first, t[1].second, color_);
	}
};

class Box : public Object
{
protected:
	float x1_;
	float y1_;
	float x2_;
	float y2_;

public:

	enum {NW = 0, NE = 1, SW = 2, SE = 3};

	Box() : Object() {};

	Box(float x1, float y1, float x2, float y2)
	{
		shape_ = BOX;

		bool leftmostX = x1 < x2 ? true : false;
		bool uppermostY = y1 < y2 ? true : false;

		float startX = leftmostX == true ? x1 : x2;
		float startY = uppermostY == true ? y1 : y2;

		px_ = fabs(x1 - x2) / 2 + startX;
		py_ = fabs(y1 - y2) / 2 + startY;
	
		x1_ = startX;
		y1_ = startY;

		x2_ = leftmostX == true ? x2 : x1;
		y2_ = uppermostY == true ? y2 : y1;

		objectModel_.push_back({x1_, y1_});
		objectModel_.push_back({x2_, y1_});
		objectModel_.push_back({x1_, y2_});
		objectModel_.push_back({x2_, y2_});
	}

	virtual void resizeObject(int cornor, iPair mouse)
	{

		if(cornor >= 0 && cornor < 4)
		{		
			float Length = objectModel_[NE].first - objectModel_[NW].first;
			float Height = objectModel_[SW].second - objectModel_[NW].second;
			float offsetX = px_ - Length / 2;
			float offsetY = py_ - Height / 2;

			fPair castedMouse = static_cast<fPair>(mouse);
			switch(cornor)
			{
				case NW :
				objectModel_[NW] = castedMouse;
				objectModel_[NE].second = castedMouse.second;
				objectModel_[SW].first = castedMouse.first;
				break;

				case NE :
				objectModel_[NE] = castedMouse;
				objectModel_[NW].second = castedMouse.second;
				objectModel_[SE].first = castedMouse.first;
				break;

				case SW :
				objectModel_[SW] = castedMouse;
				objectModel_[SE].second = castedMouse.second;
				objectModel_[NW].first = castedMouse.first;
				break;

				case SE :
				objectModel_[SE] = castedMouse;
				objectModel_[SW].second = castedMouse.second;
				objectModel_[NE].first = castedMouse.first;
				break;
			}

			//px_ = Length / 2 + objectModel_[NW].first;
			//py_ = Height / 2 + objectModel_[NE].second;
		}


	}

	virtual void drawShape(olc::PixelGameEngine * engine) override
	{
		//std::vector<fPair> t = translatePolygon(objectModel_, px_, py_, rotation_, 1.0f);
		//objectModel_ = t;
		std::vector<fPair> t = objectModel_;

		px_ = fabs(objectModel_[NE].first - objectModel_[NW].first) / 2 + objectModel_[NW].first;
		py_ = fabs(objectModel_[SW].second - objectModel_[NW].second) / 2 + objectModel_[NW].second;
/*		int verts = t.size();
		for(int i = 0; i < verts + 1; i++)
		{
			int j = i + 1;
			LlobstidrawLine(engine, t[i % verts].first, t[i % verts].second, t[j % verts].first, t[j % verts].second, color_);
*/
			LlobstidrawLine(engine, t[NW].first, t[NW].second, t[NE].first, t[NE].second, color_);
			LlobstidrawLine(engine, t[NE].first, t[NE].second, t[SE].first, t[SE].second, color_);
			LlobstidrawLine(engine, t[SE].first, t[SE].second, t[SW].first, t[SW].second, color_);
			LlobstidrawLine(engine, t[SW].first, t[SW].second, t[NW].first, t[NW].second, color_);
		//}
	}

	virtual bool isPointInsideObject(int* cornor, fPair mouse)
	{
		int locatorX = 0;
		int locatorY = 0;
		bool isIn = false;
		float Length = objectModel_[NE].first - objectModel_[NW].first;
		float Height = objectModel_[SW].second - objectModel_[NW].second;
		float offsetX = px_ - Length / 2;
		float offsetY = py_ - Height / 2;
		//std::vector<fPair> translatedObjectModel_;
		//translatedObjectModel_.resize(objectModel_.size());
		//translatedObjectModel_ = translatePolygon(objectModel_, px_, py_, rotation_, 1.0f);

		std::cout << "mouse.first: " << mouse.first << "\tobjectModel_[NW].first: " << objectModel_[NW].first << std::endl;

		if((mouse.first > objectModel_[NW].first) && (mouse.first < ((Length / 2) + objectModel_[NW].first)))  
		{
			std::cout << "mouse.first: " << mouse.first << "\tobjectModel_[NW].first: " << objectModel_[NW].first << std::endl;
			locatorX = 0;
			isIn = true;
		}

		else if((mouse.first >= (Length / 2) + objectModel_[NW].first) && (mouse.first < objectModel_[NE].first))
		{
			locatorX = 1;
			isIn = true;
		} 

		if((mouse.second >= objectModel_[NW].second) && (mouse.second < (Height / 2) + objectModel_[NW].second)) 
		{
			locatorY = 0;
			isIn = true;
		}

		else if((mouse.second >= (Height / 2) + objectModel_[NW].second) && (mouse.second < objectModel_[SW].second))
		{
			locatorY = 2;
			isIn = true;
		}

		*cornor = locatorX + locatorY;
		return isIn;
	}
};

class Ball : public Object
{
public:

	Ball() : Object() {}

	Ball(float px, float py, float vx = 0.0f, float vy = 0.0f, float radius = 0.0f) : Object(px, py, vx, vy, radius) 
	{
		shape_ = BALL;

		if(radius_ < 0)
		{
			radius_ = sqrt((fabs(px_ - vx_) * fabs(px_ - vx_)) + (fabs(py_ - vy_) * fabs(py_ - vy_)));
			radius_ = px_ < vx_ ? radius_ : -radius_;

			rotation_ = atan2f((vy_ - py_), (vx_ - px_));
		}

		int verts = vecModel_.size();
		objectModel_.resize(verts);
		for(int i = 0; i < verts; i++)
		{
			objectModel_[i].first = vecModel_[i].first * radius_;
			objectModel_[i].second = vecModel_[i].second * radius_;
		}

	}

	virtual void drawShape(olc::PixelGameEngine * engine) override
	{
		//rotation_ = atan2f(vy_, vx_);
		std::vector<fPair> t = translatePolygon(objectModel_, px_, py_, rotation_, 1.0f);
		int verts = t.size();
		for(int i = 1; i < verts; i++)
		{
			int j = i + 1;
			LlobstidrawLine(engine, t[i % (verts - 1)].first, t[i % (verts - 1)].second, t[j % (verts - 1)].first, t[j % (verts - 1)].second, color_);
		}
	}


	void moveDirection(float stepSize)
	{


		
		float directionX = cosf(rotation_) * stepSize;
		float directionY = sinf(rotation_) * stepSize;

		px_ += directionX;
		py_ += directionY;
		
		//vx_ += directionX;
		//vy_ += directionY;
	}

	virtual bool isPointInsideObject(fPair mouse)
	{
		return sqrtf(((px_ - mouse.first) * (px_ - mouse.first)) + ((py_ - mouse.second) * (py_ - mouse.second))) < fabs(radius_) ? true : false;
	}

private:
	static std::vector<fPair> vecModel_;
};

std::vector<fPair> makeUnitCircle()
{
	int numOfPoints = 20;
	std::vector<fPair> v;
	v.push_back({ 0.0f, 0.0f });
	
	for(int i = 0; i <= numOfPoints; i++)
	{
		v.push_back({ cosf(i / (float)(numOfPoints -1) * 2.00f * 3.14159f) , sinf(i / (float)(numOfPoints - 1) * 2.0f * 3.14159f) });
	}
	return v;
}

std::vector<fPair> Ball::vecModel_ = makeUnitCircle();
