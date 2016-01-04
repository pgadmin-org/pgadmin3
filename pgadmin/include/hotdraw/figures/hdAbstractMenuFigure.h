//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAbstractMenuFigure.h - Base class for figures that show a menu with right click
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDABSTRACTMENUFIGURE_H
#define HDABSTRACTMENUFIGURE_H
#include "hotdraw/utilities/hdRect.h"
#include "hotdraw/figures/hdAttributeFigure.h"
#include "hotdraw/tools/hdITool.h"
#include "hotdraw/handles/hdIHandle.h"
#include "hotdraw/utilities/hdCollection.h"

class hdAbstractMenuFigure : public hdAttributeFigure
{
public:
	hdAbstractMenuFigure();
	~hdAbstractMenuFigure();
	virtual hdITool *CreateFigureTool(hdDrawingView *view, hdITool *defaultTool);
	virtual void createMenu(wxMenu &mnu) {}
	virtual void enablePopUp();
	virtual void disablePopUp();
	virtual bool menuEnabled();
	virtual void OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view = NULL);
protected:
	wxArrayString strings;
	bool showMenu;
private:

};
#endif
