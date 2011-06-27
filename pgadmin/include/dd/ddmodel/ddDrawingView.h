//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawingEditor.h - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDDRAWINGVIEW_H
#define DDDRAWINGVIEW

#include "dd/wxhotdraw/main/wxhdDrawingView.h"

class ddDrawingView : public wxhdDrawingView
{
public:
	ddDrawingView(wxWindow *ddParent, wxhdDrawingEditor *editor ,wxSize size, wxhdDrawing *drawing);
	virtual void deleteSelectedFigures();
protected:
private:
};
#endif
