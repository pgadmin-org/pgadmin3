//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAttributeFigure.cpp - Base class for all figures with attributes (line size, fonts and others)
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "dd/wxhotdraw/figures/wxhdAttributeFigure.h"
#include "dd/wxhotdraw/figures/wxhdAttribute.h"
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdFillAttribute.h"
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdFontAttribute.h"
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdFontColorAttribute.h"
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdLineAttribute.h"

wxhdAttributeFigure::wxhdAttributeFigure()
{
	initializeDefaultAttributes();
}

wxhdAttributeFigure::~wxhdAttributeFigure()
{
	delete fillAttribute;
	delete lineAttribute;
	delete fontAttribute;
	delete fontColorAttribute;
	delete fillSelAttribute;
	delete lineSelAttribute;
	delete fontSelAttribute;
	delete fontSelColorAttribute;
}

void wxhdAttributeFigure::draw(wxBufferedDC& context, wxhdDrawingView *view)
{
	//find a way to allow user to use custom attributes without affecting performance	
	fillAttribute->apply(context);
	lineAttribute->apply(context);
	fontAttribute->apply(context);
	fontColorAttribute->apply(context);
	
	wxhdAbstractFigure::draw(context, view);
}

void wxhdAttributeFigure::drawSelected(wxBufferedDC& context, wxhdDrawingView *view)
{
	//find a way to allow user to use custom attributes without affecting performance	
	fillSelAttribute->apply(context);
	lineSelAttribute->apply(context);
	fontSelAttribute->apply(context);
	fontSelColorAttribute->apply(context);

	wxhdAbstractFigure::drawSelected(context, view);
}

void wxhdAttributeFigure::initializeDefaultAttributes()
{

	fontAttribute=new wxhdFontAttribute();
	fontColorAttribute=new wxhdFontColorAttribute();
	fillAttribute=new wxhdFillAttribute();
	lineAttribute=new wxhdLineAttribute();

	fontSelAttribute=new wxhdFontAttribute();
	fontSelColorAttribute=new wxhdFontColorAttribute();
	fillSelAttribute=new wxhdFillAttribute();
	lineSelAttribute=new wxhdLineAttribute();

}

void wxhdAttributeFigure::reapplyAttributes(wxBufferedDC& context, wxhdDrawingView *view)
{
	fillAttribute->apply(context);
	lineAttribute->apply(context);
	fontAttribute->apply(context);
	fontColorAttribute->apply(context);
}

void wxhdAttributeFigure::reapplySelAttributes(wxBufferedDC& context, wxhdDrawingView *view)
{
	fillSelAttribute->apply(context);
	lineSelAttribute->apply(context);
	fontSelAttribute->apply(context);
	fontSelColorAttribute->apply(context);
}