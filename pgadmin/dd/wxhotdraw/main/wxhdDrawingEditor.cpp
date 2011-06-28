//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdDrawingEditor.cpp - Main class that manages all other classes
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/wxhotdraw/main/wxhdDrawingEditor.h"
#include "dd/wxhotdraw/main/wxhdDrawingView.h"
#include "dd/wxhotdraw/tools/wxhdITool.h"

wxhdDrawingEditor::wxhdDrawingEditor(wxWindow *owner, bool defaultView)
{
	_model = new wxhdDrawing();
	if(defaultView)
		createView(owner);
	_tool = NULL;
}

wxhdDrawingEditor::~wxhdDrawingEditor()
{
	if(_view)
		delete _view;
	if(_model)
		delete _model;
	if(_tool)
		delete _tool;
}

//Hack to allow create different kind of custom views inside custom editor
void wxhdDrawingEditor::createView(wxWindow *owner)
{
	_view = new wxhdDrawingView(owner, this, wxSize(1200, 1200), _model);
	// Set Scroll Bar & split
	_view->SetScrollbars( 10, 10, 127, 80 );
	_view->EnableScrolling(true, true);
	_view->AdjustScrollbars();
}

wxhdDrawingView *wxhdDrawingEditor::view()
{
	return _view;
}

wxhdDrawing *wxhdDrawingEditor::model()
{
	return _model;
}

wxhdITool *wxhdDrawingEditor::tool()
{
	return _tool;
}

void wxhdDrawingEditor::setTool(wxhdITool *tool)
{
	if(_tool)
		delete _tool;
	_tool = tool;
}

void wxhdDrawingEditor::createMenu(wxMenu &mnu)
{
	wxMenuItem *item;
	item = mnu.AppendCheckItem(1000, _("Sample Item"));
	item->Check(true);
}

void wxhdDrawingEditor::OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view)
{
	switch(event.GetId())
	{
		case 1000:
			wxMessageBox(_("Sample menu item"), _("Sample"), wxOK, view);
	}
}