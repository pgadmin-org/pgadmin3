//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdFillAttribute.h - Default attribute for fill color of figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDFILLCOLORATTRIBUTE_H
#define WXHDFILLCOLORATTRIBUTE_H

#include "dd/wxhotdraw/figures/wxhdAttribute.h"

class wxhdFillAttribute : public wxhdAttribute
{
public:
	wxhdFillAttribute();
	virtual void callDefaultChangeDialog(wxWindow *owner = NULL);
	virtual void apply(wxBufferedDC &context);
	wxBrush &brush();
protected:
	wxBrush fillAttributes;
};
#endif
