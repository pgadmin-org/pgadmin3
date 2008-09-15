//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsStmt.cpp,v 1.3 2008/08/10 19:50:52 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsStmt.h"

pgsStmt::pgsStmt(pgsThread * app) :
	m_line(0), m_app(app)
{

}

pgsStmt::~pgsStmt()
{

}

void pgsStmt::set_position(int line)
{
	m_line = line;
}

int pgsStmt::line() const
{
	return m_line;
}
