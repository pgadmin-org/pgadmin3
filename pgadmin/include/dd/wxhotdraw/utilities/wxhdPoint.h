//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdPoint.h - wxPoint class to be used as wrapper and allow independence of point class
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDPOINT_H
#define WXHDPOINT_H

class wxhdPoint : public wxPoint{
public:
	wxhdPoint();
	wxhdPoint(int x, int y);
	wxhdPoint(const wxhdPoint& p);
	wxhdPoint(const wxPoint& p);
};

#endif
