//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddColumnOptionIcon.h -
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDCOLUMNOPTIONICON_H
#define DDCOLUMNOPTIONICON_H

#include "hotdraw/figures/hdAbstractMenuFigure.h"

class ddColumnFigure;

enum ddColumnOptionType
{
	null = 321,
	notnull
};

enum
{
	MNU_COLNULL = 321,
	MNU_COLNOTNULL
};

class ddColumnOptionIcon : public hdAbstractMenuFigure
{
public:
	ddColumnOptionIcon(ddColumnFigure *owner);
	~ddColumnOptionIcon();
	virtual void OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view = NULL);
	virtual void createMenu(wxMenu &mnu);
	virtual void basicDraw(wxBufferedDC &context, hdDrawingView *view);
	virtual void basicDrawSelected(wxBufferedDC &context, hdDrawingView *view);
	virtual void changeIcon(ddColumnOptionType type);
	virtual int getWidth();
	virtual int getHeight();
	ddColumnOptionType getOption();
	ddColumnFigure *getOwnerColumn();

protected:

private:
	ddColumnOptionType colOption;
	ddColumnFigure *ownerColumn;
	wxBitmap *iconToDraw;
	wxBitmap icon;
};
#endif
