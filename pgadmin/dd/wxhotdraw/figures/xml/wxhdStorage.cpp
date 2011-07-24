//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// wxhdStorage.cpp - Base class for managing all figures persistence
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// libxml2 headers
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

// App headers
#include "dd/wxhotdraw/figures/xml/wxhdStorage.h"

wxhdStorage::wxhdStorage():
	wxhdObject()
{
}

bool wxhdStorage::Read(xmlTextReaderPtr reader)
{
	return false;
}

bool wxhdStorage::Write(xmlTextWriterPtr writer, wxhdIFigure *figure)
{
	return false;
}
