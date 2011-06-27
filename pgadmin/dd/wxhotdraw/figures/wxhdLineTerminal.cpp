//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdLineTerminal.cpp - Base class for line terminal figure
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdLineTerminal.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/utilities/wxhdGeometry.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"

wxhdLineTerminal::wxhdLineTerminal()
{
	middle = wxhdPoint(0,0);
	terminalLinePen = wxPen(wxString(wxT("BLACK")),1,wxSOLID);
}

wxhdLineTerminal::~wxhdLineTerminal()
{
}

void wxhdLineTerminal::setLinePen(wxPen pen)
{
	terminalLinePen=pen;
}

wxhdPoint& wxhdLineTerminal::draw (wxBufferedDC& context, wxhdPoint& a, wxhdPoint& b, wxhdDrawingView *view)
{
	wxhdGeometry g;
	context.SetPen(terminalLinePen);

	wxhdPoint copyA = wxhdPoint (a);
	view->CalcScrolledPosition(copyA.x,copyA.y,&copyA.x,&copyA.y);
	wxhdPoint copyB = wxhdPoint (b);
	view->CalcScrolledPosition(copyB.x,copyB.y,&copyB.x,&copyB.y);
	context.DrawLine(copyA, copyB);
	
	context.SetPen(wxPen(wxString(wxT("BLACK")),1,wxSOLID));
	int x=copyA.x+g.ddabs(copyA.x - copyB.x);
	int y=copyA.y+g.ddabs(copyA.y - copyB.y);
	middle = wxhdPoint(x,y);

	context.DrawRectangle(wxRect(copyA.x,copyA.y,5,5));
	context.DrawCircle(copyA,10);
	return middle;
}
