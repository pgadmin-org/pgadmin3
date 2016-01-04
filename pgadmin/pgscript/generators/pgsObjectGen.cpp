//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/generators/pgsObjectGen.h"

pgsObjectGen::pgsObjectGen(const long &seed) :
	m_seed(seed == 0 ? 1 : seed)
{

}

pgsObjectGen::~pgsObjectGen()
{

}
