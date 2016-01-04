//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsOperation.h"

#include "pgscript/objects/pgsVariable.h"

pgsOperation::pgsOperation(const pgsExpression *left, const pgsExpression *right) :
	pgsExpression(), m_left(left), m_right(right)
{

}

pgsOperation::~pgsOperation()
{
	pdelete(m_left);
	pdelete(m_right);
}

pgsOperation::pgsOperation(const pgsOperation &that) :
	pgsExpression(that)
{
	m_left = that.m_left->clone();
	m_right = that.m_right != 0 ? that.m_right->clone() : 0;
}

pgsOperation &pgsOperation::operator =(const pgsOperation &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		pdelete(m_left);
		pdelete(m_right);
		m_left = that.m_left->clone();
		m_right = that.m_right != 0 ? that.m_right->clone() : 0;
	}
	return (*this);
}
