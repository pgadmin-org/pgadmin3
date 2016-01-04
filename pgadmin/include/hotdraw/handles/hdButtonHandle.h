//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdButtonHandle.h - Handle to allow creation of buttons at figures
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDBUTTONHANDLE_H
#define HDBUTTONHANDLE_H

#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/handles/hdLocatorHandle.h"
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/utilities/hdPoint.h"

class hdButtonHandle : public hdIHandle
{
public:
	hdButtonHandle(hdIFigure *owner, hdILocator *buttonLocator , wxBitmap &buttonImage, wxSize &size);
	~hdButtonHandle();

	virtual wxCursor createCursor();
	virtual hdRect &getDisplayBox(int posIdx);
	virtual void draw(wxBufferedDC &context, hdDrawingView *view);
	virtual hdPoint &locate(int posIdx);
	virtual void invokeStart(hdMouseEvent &event, hdDrawingView *view) = 0;
	virtual void invokeStep(hdMouseEvent &event, hdDrawingView *view) = 0;
	virtual void invokeEnd(hdMouseEvent &event, hdDrawingView *view) = 0;
protected:
	wxBitmap buttonIcon;
private:
	bool clicked;
	hdILocator *bLocator;
	hdPoint pointLocate;

};
#endif
