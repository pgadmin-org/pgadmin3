//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// hdStorage.cpp - Base class for managing all figures persistence
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// libxml2 headers
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

// App headers
#include "hotdraw/figures/xml/hdStorage.h"

hdStorage::hdStorage():
	hdObject()
{
}

bool hdStorage::Read(xmlTextReaderPtr reader)
{
	return false;
}

bool hdStorage::Write(xmlTextWriterPtr writer, hdIFigure *figure)
{
	return false;
}
