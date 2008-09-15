//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsObjectGen.cpp,v 1.2 2008/08/10 17:45:37 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsObjectGen.h"

pgsObjectGen::pgsObjectGen(const long & seed) :
	m_seed(seed == 0 ? 1 : seed)
{

}

pgsObjectGen::~pgsObjectGen()
{

}
