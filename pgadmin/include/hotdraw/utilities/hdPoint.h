//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdPoint.h - wxPoint class to be used as wrapper and allow independence of point class
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDPOINT_H
#define HDPOINT_H

class hdPoint : public wxPoint
{
public:
	hdPoint();
	hdPoint(int x, int y);
	hdPoint(const hdPoint &p);
	hdPoint(const wxPoint &p);
};

#endif
