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

#ifndef HDATTRIBUTE_H
#define HDATTRIBUTE_H
#include "hotdraw/main/hdObject.h"

class hdAttribute : public hdObject
{
public:
	hdAttribute();
	virtual void callDefaultChangeDialog(wxWindow *owner = NULL);
	virtual void apply(wxBufferedDC &context);
};
#endif
