//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdGeometry.cpp - Utility Geometric Functions Shared between classes
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "hotdraw/utilities/hdGeometry.h"
#include "hotdraw/utilities/hdMultiPosRect.h"
#include "hotdraw/utilities/hdPoint.h"

//
// Warning when using it:  typecasting to avoid miscalculations functions need double values not int
//

bool hdGeometry::lineContainsPoint(double x1, double y1, double x2, double y2, double px, double py)
{
	hdPoint p = hdPoint(x1, y1);
	hdRect r = hdRect(p);
	r.add(x2, y2);
	r.Inflate(2, 2);
	if(!r.Contains(px, py))
	{
		return false;
	}

	double a, b, x, y;
	double val1, val2;

	val1 = px - x1;
	if( x1 == x2 )
	{
		return (ddabs(val1) < 3);
	}

	val2 = py - y1;
	if( y1 == y2 )
	{
		return (ddabs(val2) < 3);
	}

	a = (y1 - y2) / (x1 - x2);
	b = y1 - a * x1;
	x = (py - b) / a;
	y = a * px + b;

	val1 = x - px;
	val2 = y - py;
	bool out = (min( ddabs(val1), ddabs(val2)) < 4);
	return out;
}

int hdGeometry::min(double a, double b)
{
	return(a <= b) ? a : b;
}

int hdGeometry::max(double a, double b)
{
	return(a >= b) ? a : b;
}


int hdGeometry::min(int a, int b)
{
	return(a <= b) ? a : b;
}

int hdGeometry::max(int a, int b)
{
	return(a >= b) ? a : b;
}

//Gets the angle of a point relative to a rectangle.
double hdGeometry::angleFromPoint(int posIdx, hdMultiPosRect r, hdPoint point)
{
	return angleFromPoint(r.gethdRect(posIdx), point);
}

double hdGeometry::angleFromPoint(hdRect r, hdPoint point)
{
	double rx = point.x - r.center().x;
	double ry = point.y - r.center().y;
	return atan2 (ry * r.width, rx * r.height);
}

hdPoint hdGeometry::edgePointFromAngle(int posIdx, hdMultiPosRect r, double angle)
{
	return edgePointFromAngle(r.gethdRect(posIdx), angle);
}

//Gets the point on a rectangle that corresponds to the given angle.
hdPoint hdGeometry::edgePointFromAngle(hdRect r, double angle)
{
	static hdPoint locationPoint; //Hack to allow bug in linux & ddabs
	double sinv = sin(angle);
	double cosv = cos(angle);
	double e = 0.0001;
	double x = 0.0;
	double y = 0.0;
	double width = r.width;
	double height = r.height;


	if( ddabs(sinv) > e )
	{
		x = (1.0 + cosv / ddabs (sinv)) / 2.0 * width;
		x = range(0.0, width, x);
	}
	else if ( cosv >= 0.0 )
	{
		x = width;
	}

	if ( ddabs(cosv) > e )
	{
		y = (1.0 + sinv / ddabs (cosv)) / 2.0 * height;
		y = range (0.0, height, y);
	}
	else if ( sinv >= 0.0 )
	{
		y = height;
	}
	int xx = r.x + x;
	int yy = r.y + y;
	locationPoint = hdPoint(xx, yy);
	return locationPoint;
}

double hdGeometry::range(double min, double max, double num)
{
	return num < min ? min : (num > max ? max : num);
}

double hdGeometry::ddabs(double value)
{
	return value < 0 ? (value * -1) : value;
}

int hdGeometry::ddabs(int value)
{
	return value < 0 ? (value * -1) : value;
}

double hdGeometry::lineSize (hdPoint p1, hdPoint p2)
{
	int w = p1.x - p2.x;
	int h = p1.y - p2.y;

	double perimeter = w * w + h * h;
	return sqrt (perimeter);
}

// source: http://vision.dai.ed.ac.uk/andrewfg/c-g-a-faq.html
// Standard line intersection algorithm, Return true intersection if it exists, else false.
bool hdGeometry::intersection(hdPoint p1, hdPoint p2, hdPoint p3, hdPoint p4)
{
	// Store the values for fast access and easy
	// equations-to-code conversion
	float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
	float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (d == 0) return false;

	// Get the x and y
	float pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
	// point of intersection
	float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

	// Check if the x and y coordinates are within both lines
	if ( x < min(x1, x2) || x > max(x1, x2) || x < min(x3, x4) || x > max(x3, x4) )
		return false;
	if ( y < min(y1, y2) || y > max(y1, y2) || y < min(y3, y4) || y > max(y3, y4) )
		return false;

	return true;
}
