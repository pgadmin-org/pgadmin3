//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdLineAttribute.h - Default Attribute for lines style, color an others at figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef WXHDLINECOLORATTRIBUTE_H
#define WXHDLINECOLORATTRIBUTE_H

#include "dd/wxhotdraw/figures/wxhdAttribute.h"

class wxhdLineAttribute : public wxhdAttribute
{
public:
	wxhdLineAttribute();
	virtual void callDefaultChangeDialog(wxWindow *owner = NULL);
	virtual void apply(wxBufferedDC &context);
	wxPen &pen();
protected:
	wxPen penAttributes;
};
#endif
