//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdSimpleTextFigure.cpp - A simple rectangle  figure with text inside it
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdSimpleTextFigure.h"
#include "dd/wxhotdraw/tools/wxhdSimpleTextTool.h"
#include "dd/wxhotdraw/utilities/wxhdGeometry.h"

// dummy image
#include "images/ddnull.pngc"

wxhdSimpleTextFigure::wxhdSimpleTextFigure(wxString textString)
{
	textEditable = false;
	font = settings->GetSystemFont();
	padding = 2;
	setText(textString);
	showMenu = false;
}

wxhdSimpleTextFigure::~wxhdSimpleTextFigure()
{
}

void wxhdSimpleTextFigure::setText(wxString textString)
{
	text = textString;
	recalculateDisplayBox();
}

//extended is flag that inform about returning an extended version of text stored at figure
wxString &wxhdSimpleTextFigure::getText(bool extended)
{
	return text;
}

void wxhdSimpleTextFigure::setFont(wxFont textFont)
{
	font = textFont;
	recalculateDisplayBox();
}

void wxhdSimpleTextFigure::getFontMetrics(int &width, int &height)
{
	wxBitmap emptyBitmap(*ddnull_png_img);
	wxMemoryDC temp_dc;
	temp_dc.SelectObject(emptyBitmap);
	temp_dc.SetFont(font);
	if(getText(true).length() > 5)
		temp_dc.GetTextExtent(getText(true), &width, &height);
	else
		temp_dc.GetTextExtent(wxT("EMPTY"), &width, &height);
}

void wxhdSimpleTextFigure::recalculateDisplayBox()
{
	int w, h;

	getFontMetrics(w, h);

	wxhdGeometry g;
	displayBox().width = g.max(w, 10) + padding;
	displayBox().height = g.max(h, 10) + padding;
}

void wxhdSimpleTextFigure::basicDraw(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdRect copy = displayBox();
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	context.DrawText(getText(true), copy.GetPosition());
}

void wxhdSimpleTextFigure::basicDrawSelected(wxBufferedDC &context, wxhdDrawingView *view)
{
	wxhdRect copy = displayBox();
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	context.DrawText(getText(true), copy.GetPosition());
}

void wxhdSimpleTextFigure::basicMoveBy(int x, int y)
{
	displayBox().x += x;
	displayBox().y += y;
}

wxhdITool *wxhdSimpleTextFigure::CreateFigureTool(wxhdDrawingEditor *editor, wxhdITool *defaultTool)
{
	return textEditable ? new wxhdSimpleTextTool(editor, this, defaultTool) : defaultTool;
}

void wxhdSimpleTextFigure::setEditable(bool value)
{
	textEditable = value;
}

bool wxhdSimpleTextFigure::getEditable()
{
	return textEditable;
}

int wxhdSimpleTextFigure::getPadding()
{
	return padding;
}

void wxhdSimpleTextFigure::enablePopUp()
{
	showMenu = true;
}

void wxhdSimpleTextFigure::disablePopUp()
{
	showMenu = false;
}

bool wxhdSimpleTextFigure::menuEnabled()
{
	return 	showMenu;
}

void wxhdSimpleTextFigure::OnGenericPopupClick(wxCommandEvent &event, wxhdDrawingView *view)
{
	setText(strings[event.GetId()]);
}
