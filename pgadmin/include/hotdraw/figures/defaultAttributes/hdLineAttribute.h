//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdLineAttribute.h - Default Attribute for lines style, color an others at figure
//
//////////////////////////////////////////////////////////////////////////

#ifndef HDLINECOLORATTRIBUTE_H
#define HDLINECOLORATTRIBUTE_H

#include "hotdraw/figures/hdAttribute.h"

class hdLineAttribute : public hdAttribute
{
public:
	hdLineAttribute();
	virtual void callDefaultChangeDialog(wxWindow *owner = NULL);
	virtual void apply(wxBufferedDC &context);
	wxPen &pen();
protected:
	wxPen penAttributes;
};
#endif
