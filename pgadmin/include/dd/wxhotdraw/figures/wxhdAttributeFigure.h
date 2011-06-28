//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdAbstractFigure.h - Base class for all figures with attributes (line size, fonts and others)
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDATTRIBUTEFIGURE_H
#define WXHDATTRIBUTEFIGURE_H

// wxWindows headers
#include "dd/wxhotdraw/figures/wxhdAbstractFigure.h"
#include "dd/wxhotdraw/figures/wxhdAttribute.h"
#include "dd/wxhotdraw/main/wxhdObject.h"
#include <wx/arrstr.h>

// App headers
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdFillAttribute.h"
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdFontAttribute.h"
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdFontColorAttribute.h"
#include "dd/wxhotdraw/figures/defaultAttributes/wxhdLineAttribute.h"

class wxhdAttributeFigure : public wxhdAbstractFigure
{
public:
	wxhdAttributeFigure();
	~wxhdAttributeFigure();

	void draw(wxBufferedDC &context, wxhdDrawingView *view);
	void drawSelected(wxBufferedDC &context, wxhdDrawingView *view);
	void reapplyAttributes(wxBufferedDC &context, wxhdDrawingView *view);
	void reapplySelAttributes(wxBufferedDC &context, wxhdDrawingView *view);

	void initializeDefaultAttributes();

	//Draw attributes
	wxhdFontAttribute *fontAttribute;
	wxhdFontColorAttribute *fontColorAttribute;
	wxhdFillAttribute *fillAttribute;
	wxhdLineAttribute *lineAttribute;
	//Draw selected attributes
	wxhdFontAttribute *fontSelAttribute;
	wxhdFontColorAttribute *fontSelColorAttribute;
	wxhdFillAttribute *fillSelAttribute;
	wxhdLineAttribute *lineSelAttribute;
protected:

private:

};
#endif
