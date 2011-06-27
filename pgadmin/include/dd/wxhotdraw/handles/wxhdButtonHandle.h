//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdButtonHandle.h - Handle to allow creation of buttons at figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDBUTTONHANDLE_H
#define WXHDBUTTONHANDLE_H

#include "dd/wxhotdraw/handles/wxhdIHandle.h"
#include "dd/wxhotdraw/handles/wxhdLocatorHandle.h"
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/utilities/wxhdPoint.h"

class wxhdButtonHandle : public wxhdIHandle
{
public:
	wxhdButtonHandle(wxhdIFigure *owner, wxhdILocator *buttonLocator ,wxBitmap &buttonImage, wxSize &size);
    ~wxhdButtonHandle();

	virtual wxCursor createCursor();
	virtual wxhdRect& getDisplayBox();
	virtual void draw(wxBufferedDC& context, wxhdDrawingView *view);
	virtual wxhdPoint& locate();
	virtual void invokeStart(wxhdMouseEvent& event, wxhdDrawingView *view)=0;
	virtual void invokeStep(wxhdMouseEvent& event, wxhdDrawingView *view)=0;
	virtual void invokeEnd(wxhdMouseEvent& event, wxhdDrawingView *view)=0;
protected:
	wxBitmap buttonIcon;
private:
	bool clicked;
	wxhdILocator *bLocator;
	wxhdPoint pointLocate;

};
#endif
