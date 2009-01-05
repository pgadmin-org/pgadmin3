//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsNumberGen.h"

pgsNumberGen::pgsNumberGen(const MAPM & range) :
	m_range(range)
{

}

pgsNumberGen::~pgsNumberGen()
{

}
