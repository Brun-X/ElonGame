/*

IMPLEMNTED WITH THE OLCPIXELGAMEENGINE FROM:
		    
		    www.onelonecoder.com

		  GREAT THANKS TO JAVIDX9

*/

#define OLC_PGE_APPLICATION

#ifndef OBJECT_HPP_
#define OBJECT_HPP_

#include <cmath>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "olcPixelGameEngine.h"




enum { MOVE=0, BALL=1, BOX=2, LINE=3 };


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
	float vx_;
	float vy_;
	float radius_;
	float rotation_ = 0.0f;
	float velocity_ = 0.0f;
	float acceleration_ = 1.0f;
	olc::Pixel color_ = olc::WHITE;
	
	virtual ~Object() {}

	virtual void calculateShape() {}

	virtual void drawShape() {}

	static bool LlobstidrawLine(float x1, float y1, float x2, float y2, olc::Pixel color)
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

		for(auto l : line)
		{
			draw(line.first, line.second, color);
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

	virtual void drawShape() override
	{
		std::vector<fPair> t = translatePolygon(objectModel_, px_, py_, 0.0f, 1.0f)
		LlobstidrawLine(t[0].first, t[0].second, t[1].first, t[1].second, color_);
	}
};

class Box : public Object
{
public:
	Box() : Object() {};

	Box(float x1, float y1, float x2, float y2)
	{
		shape_ = BOX;
	
		objectModel_.push_back({x1, y1});
		objectModel_.push_back({x2, y1});
		objectModel_.push_back({x2, y2});
		objectModel_.push_back({x1, y2});

		/*
		makeLine(x1_, y1_, x2_, y1_, &objectLines_);
		makeLine(x2_, y1_, x2_, y2_, &objectLines_);
		makeLine(x2_, y2_, x1_, y2_, &objectLines_);
		makeLine(x1_, y2_, x1_, y1_, &objectLines_);
		*/
		
	}

	virtual void calculateShape() override {}

	virtual void drawShape() override
	{
		std::vector<fPair> t = translatePolygon(objectModel_, px_, py_, rotation_, 1.0f)
		int verts = t.size();
		for(int i = 0; i < verts + 1; i++)
		{
			int j = i + 1;
			LlobstidrawLine(t[i % verts].first, t[i % verts].second, t[j % verts].first, t[j % verts].second, color_);
		}
	}
};

class Ball : public Object
{
public:
	Ball() : Object() {}

	Ball(float px, float py, float vx = 0.0f, float vy = 0.0f, float radius) : px_(px), py_(py), vx_(vx), vy_(vy), radius_(radius)
	{
		shape_ = BALL;

		if(radius < 0)
		{
			radius_ = sqrt((fabs(px - vx) * fabs(px - vx)) + (fabs(py - vy) * fabs(py - vy)));
			radius_ = px_ < vx_ ? radius_ : -radius_;

			rotation_ = atan2f((vy_ - py_) / (vx_ - px_));

			/*
			rotation_ = acosf(fabs((vx_ - px_)) / radius_);
			if(vy_ > py_ && radius_ > 0.0f) rotation_ = (2 * M_PI) - rotation_;
			else if(vy_ > py_ && radius_ < 0.0f) rotation_ = -rotation_;
			*/
		}

		else if(radius_ > 0.0f)
		{
			vx_ = px_ + radius_;
			vy_ = py_ + radius_;
		}

		int verts = vecModel_.size();
		objectModel_.resize(verts);
		for(int i = 0; i < verts; i++)
		{
			objectModel_[i].first = vecModel_[i].first * radius_;
			objectModel_[i].second = vecModel_[i].second * radius_;
		}

	}

	virtual void calculateShape() override 
	{
		/*
		float pointX, pointY;
		float lastX = vecModel_[1].first * radius_ + px_;
		float lastY = vecModel_[1].second * radius_ + py_;
		objectLines_.clear();
*/
		/*
		for(int s = 0; s < circlePoints + 1; s++)
		{
			if(s > 1) 
			{
				pointX = (vecModel_[s + 1].first * radius_) + px_;
				pointY = (vecModel_[s + 1].second * radius_) + py_;
				
				makeLine(lastX, lastY, pointX, pointY, &objectLines_);
				
				pointX = s < circlePoints + 1 ? pointX : vecModel_[1].first * radius_ + px_;
				pointY = s < circlePoints + 1 ? pointY : vecModel_[1].second * radius_ + py_;
				lastX = pointX;
				lastY = pointY;
			}
			else
			{
				makeLine(px_, py_, vx_, vy_, &objectLines_);
			} 	
		}*/
	}

	virtual void drawShape() override
	{
		std::vector<fPair> t = translatePolygon(objectModel_, px_, py_, rotation_, 1.0f)
		int verts = t.size();
		for(int i = 0; i < verts + 1; i++)
		{
			int j = i + 1;
			LlobstidrawLine(t[i % verts].first, t[i % verts].second, t[j % verts].first, t[j % verts].second, color_);
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
/*	float px_;
	float py_;
	float vx_;
	float vy_;
	float radius_;
	float rotation_;
	float velocity_ = 0.0f;
	float acceleration_ = 1.0f;
*/
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

#endif //_OBJECT_HPP