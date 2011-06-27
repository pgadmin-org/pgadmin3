//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdGeometry.h - Utility Geometric Functions Shared between classes
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDGEOMETRY_H
#define WXHDGEOMETRY_H
#include "dd/wxhotdraw/main/wxhdObject.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"

class wxhdGeometry : public wxhdObject
{
public:
	static bool lineContainsPoint(double x1, double y1, double x2, double y2, double px, double py);
	static int min(int a, int b);
	static int max(int a, int b);
	static int min(double a, double b);
	static int max(double a, double b);
	static double angleFromPoint(wxhdRect r, wxhdPoint point);
	static wxhdPoint edgePointFromAngle(wxhdRect r, double angle);
	static double range(double min, double max, double num);
	static double lineSize (wxhdPoint p1, wxhdPoint p2);
	static bool intersection(wxhdPoint p1, wxhdPoint p2, wxhdPoint p3, wxhdPoint p4);
	static double ddabs(double value);
	static int ddabs(int value);

private:

};
#endif
