//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAttributeFigure.cpp - Base class for all figures with attributes (line size, fonts and others)
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/pen.h>

// App headers
#include "hotdraw/figures/hdAttributeFigure.h"
#include "hotdraw/figures/hdAttribute.h"
#include "hotdraw/figures/defaultAttributes/hdFillAttribute.h"
#include "hotdraw/figures/defaultAttributes/hdFontAttribute.h"
#include "hotdraw/figures/defaultAttributes/hdFontColorAttribute.h"
#include "hotdraw/figures/defaultAttributes/hdLineAttribute.h"

hdAttributeFigure::hdAttributeFigure()
{
	initializeDefaultAttributes();
}

hdAttributeFigure::~hdAttributeFigure()
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

void hdAttributeFigure::draw(wxBufferedDC &context, hdDrawingView *view)
{
	//find a way to allow user to use custom attributes without affecting performance
	fillAttribute->apply(context);
	lineAttribute->apply(context);
	fontAttribute->apply(context);
	fontColorAttribute->apply(context);

	hdAbstractFigure::draw(context, view);
}

void hdAttributeFigure::drawSelected(wxBufferedDC &context, hdDrawingView *view)
{
	//find a way to allow user to use custom attributes without affecting performance
	fillSelAttribute->apply(context);
	lineSelAttribute->apply(context);
	fontSelAttribute->apply(context);
	fontSelColorAttribute->apply(context);

	hdAbstractFigure::drawSelected(context, view);
}

void hdAttributeFigure::initializeDefaultAttributes()
{

	fontAttribute = new hdFontAttribute();
	fontColorAttribute = new hdFontColorAttribute();
	fillAttribute = new hdFillAttribute();
	lineAttribute = new hdLineAttribute();

	fontSelAttribute = new hdFontAttribute();
	fontSelColorAttribute = new hdFontColorAttribute();
	fillSelAttribute = new hdFillAttribute();
	lineSelAttribute = new hdLineAttribute();

}

void hdAttributeFigure::reapplyAttributes(wxBufferedDC &context, hdDrawingView *view)
{
	fillAttribute->apply(context);
	lineAttribute->apply(context);
	fontAttribute->apply(context);
	fontColorAttribute->apply(context);
}

void hdAttributeFigure::reapplySelAttributes(wxBufferedDC &context, hdDrawingView *view)
{
	fillSelAttribute->apply(context);
	lineSelAttribute->apply(context);
	fontSelAttribute->apply(context);
	fontSelColorAttribute->apply(context);
}
