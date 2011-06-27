//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAbstractMenuFigure.cpp - Base class for figures that show a menu with right click
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdAbstractMenuFigure.h"
#include "dd/wxhotdraw/figures/wxhdIFigure.h"
#include "dd/wxhotdraw/utilities/wxhdArrayCollection.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"
#include "dd/wxhotdraw/tools/wxhdMenuTool.h"

wxhdAbstractMenuFigure::wxhdAbstractMenuFigure()
{
}

wxhdAbstractMenuFigure::~wxhdAbstractMenuFigure()
{
}


wxhdITool* wxhdAbstractMenuFigure::CreateFigureTool(wxhdDrawingEditor *editor, wxhdITool *defaultTool)
{
	return new wxhdMenuTool(editor,this,defaultTool);
}

void wxhdAbstractMenuFigure::enablePopUp()
{
	showMenu = true;
}

void wxhdAbstractMenuFigure::disablePopUp()
{
	showMenu = false;
}

bool wxhdAbstractMenuFigure::menuEnabled()
{
	return 	showMenu;
}

void wxhdAbstractMenuFigure::OnGenericPopupClick(wxCommandEvent& event, wxhdDrawingView *view)
{
	//Action on popup goes here
	//strings[event.GetId()]
}
