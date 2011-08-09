//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAbstractMenuFigure.h - Base class for figures that show a menu with right click
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDABSTRACTMENUFIGURE_H
#define WXHDABSTRACTMENUFIGURE_H
#include "dd/wxhotdraw/utilities/wxhdRect.h"
#include "dd/wxhotdraw/figures/wxhdAttributeFigure.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"
#include "dd/wxhotdraw/handles/wxhdIHandle.h"
#include "dd/wxhotdraw/utilities/wxhdCollection.h"

class wxhdAbstractMenuFigure : public wxhdAttributeFigure
{
public:
	wxhdAbstractMenuFigure();
	~wxhdAbstractMenuFigure();
	virtual wxhdITool *CreateFigureTool(wxhdDrawingView *view, wxhdITool *defaultTool);
	virtual void createMenu(wxMenu &mnu) {}
	virtual void enablePopUp();
	virtual void disablePopUp();
	virtual bool menuEnabled();
	virtual void OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view = NULL);
protected:
	wxArrayString strings;
	bool showMenu;
private:

};
#endif
