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

#ifndef DDDRAWINGEDITOR_H
#define DDDRAWINGEDITOR_H

#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"

enum
{
	MNU_NEWTABLE = 0
};

class ddDatabaseDesign;

class ddDrawingEditor : public wxhdDrawingEditor
{
public:
	ddDrawingEditor(wxWindow *owner, ddDatabaseDesign *design);
	virtual void createView(wxWindow *owner);
	//Hack To allow right click menu at canvas without a figure
	virtual void createMenu(wxMenu &mnu);
	virtual void OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view);
protected:
private:
	ddDatabaseDesign *databaseDesign;
};
#endif
