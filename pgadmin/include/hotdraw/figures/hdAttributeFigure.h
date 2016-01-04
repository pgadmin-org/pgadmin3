//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdAbstractFigure.h - Base class for all figures with attributes (line size, fonts and others)
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDATTRIBUTEFIGURE_H
#define HDATTRIBUTEFIGURE_H

// wxWindows headers
#include "hotdraw/figures/hdAbstractFigure.h"
#include "hotdraw/figures/hdAttribute.h"
#include "hotdraw/main/hdObject.h"
#include <wx/arrstr.h>

// App headers
#include "hotdraw/figures/defaultAttributes/hdFillAttribute.h"
#include "hotdraw/figures/defaultAttributes/hdFontAttribute.h"
#include "hotdraw/figures/defaultAttributes/hdFontColorAttribute.h"
#include "hotdraw/figures/defaultAttributes/hdLineAttribute.h"

class hdAttributeFigure : public hdAbstractFigure
{
public:
	hdAttributeFigure();
	~hdAttributeFigure();

	void draw(wxBufferedDC &context, hdDrawingView *view);
	void drawSelected(wxBufferedDC &context, hdDrawingView *view);
	void reapplyAttributes(wxBufferedDC &context, hdDrawingView *view);
	void reapplySelAttributes(wxBufferedDC &context, hdDrawingView *view);

	void initializeDefaultAttributes();

	//Draw attributes
	hdFontAttribute *fontAttribute;
	hdFontColorAttribute *fontColorAttribute;
	hdFillAttribute *fillAttribute;
	hdLineAttribute *lineAttribute;
	//Draw selected attributes
	hdFontAttribute *fontSelAttribute;
	hdFontColorAttribute *fontSelColorAttribute;
	hdFillAttribute *fillSelAttribute;
	hdLineAttribute *lineSelAttribute;
protected:

private:

};
#endif
