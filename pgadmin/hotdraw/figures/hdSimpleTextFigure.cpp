//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdSimpleTextFigure.cpp - A simple rectangle  figure with text inside it
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// App headers
#include "hotdraw/figures/hdSimpleTextFigure.h"
#include "hotdraw/tools/hdSimpleTextTool.h"
#include "hotdraw/utilities/hdGeometry.h"
#include "hotdraw/figures/defaultAttributes/hdFontAttribute.h"

// dummy image
#include "images/ddnull.pngc"

hdSimpleTextFigure::hdSimpleTextFigure(wxString textString)
{
	textEditable = false;
	font = *hdFontAttribute::defaultFont;
	padding = 2;
	setText(textString);
	showMenu = false;
}

hdSimpleTextFigure::~hdSimpleTextFigure()
{
}

void hdSimpleTextFigure::setText(wxString textString)
{
	text = textString;
	recalculateDisplayBox();
}

//extended is flag that inform about returning an extended version of text stored at figure
wxString &hdSimpleTextFigure::getText(bool extended)
{
	return text;
}

void hdSimpleTextFigure::setFont(wxFont textFont)
{
	font = textFont;
	recalculateDisplayBox();
}

void hdSimpleTextFigure::getFontMetrics(int &width, int &height)
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

void hdSimpleTextFigure::recalculateDisplayBox()
{
	int w, h;

	getFontMetrics(w, h);

	hdGeometry g;
	displayBox().width = g.max(w, 10) + padding;
	displayBox().height = g.max(h, 10) + padding;
}

void hdSimpleTextFigure::basicDraw(wxBufferedDC &context, hdDrawingView *view)
{
	hdRect copy = displayBox().gethdRect(view->getIdx());
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	context.DrawText(getText(true), copy.GetPosition());
}

void hdSimpleTextFigure::basicDrawSelected(wxBufferedDC &context, hdDrawingView *view)
{
	hdRect copy = displayBox().gethdRect(view->getIdx());
	view->CalcScrolledPosition(copy.x, copy.y, &copy.x, &copy.y);
	context.DrawText(getText(true), copy.GetPosition());
}

void hdSimpleTextFigure::basicMoveBy(int posIdx, int x, int y)
{
	displayBox().x[posIdx] += x;
	displayBox().y[posIdx] += y;
}

hdITool *hdSimpleTextFigure::CreateFigureTool(hdDrawingView *view, hdITool *defaultTool)
{
	return textEditable ? new hdSimpleTextTool(view, this, defaultTool) : defaultTool;
}

void hdSimpleTextFigure::setEditable(bool value)
{
	textEditable = value;
}

bool hdSimpleTextFigure::getEditable()
{
	return textEditable;
}

int hdSimpleTextFigure::getPadding()
{
	return padding;
}

void hdSimpleTextFigure::enablePopUp()
{
	showMenu = true;
}

void hdSimpleTextFigure::disablePopUp()
{
	showMenu = false;
}

bool hdSimpleTextFigure::menuEnabled()
{
	return 	showMenu;
}

void hdSimpleTextFigure::OnGenericPopupClick(wxCommandEvent &event, hdDrawingView *view)
{
	setText(strings[event.GetId()]);
}
