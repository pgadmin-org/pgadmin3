//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdGeometry.h - Utility Geometric Functions Shared between classes
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDGEOMETRY_H
#define HDGEOMETRY_H
#include "hotdraw/main/hdObject.h"
#include "hotdraw/utilities/hdPoint.h"
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/utilities/hdMultiPosRect.h"

class hdGeometry : public hdObject
{
public:
	static bool lineContainsPoint(double x1, double y1, double x2, double y2, double px, double py);
	static int min(int a, int b);
	static int max(int a, int b);
	static int min(double a, double b);
	static int max(double a, double b);
	static double angleFromPoint(hdRect r, hdPoint point);
	static double angleFromPoint(int posIdx, hdMultiPosRect r, hdPoint point);
	static hdPoint edgePointFromAngle(hdRect r, double angle);
	static hdPoint edgePointFromAngle(int posIdx, hdMultiPosRect r, double angle);
	static double range(double min, double max, double num);
	static double lineSize (hdPoint p1, hdPoint p2);
	static bool intersection(hdPoint p1, hdPoint p2, hdPoint p3, hdPoint p4);
	static double ddabs(double value);
	static int ddabs(int value);

private:

};
#endif
