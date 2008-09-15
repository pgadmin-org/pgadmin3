//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
// RCS-ID:      $Id: pgsLowerEqual.cpp,v 1.2 2008/08/10 17:45:36 pgunittest Exp $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsLowerEqual.h"
#include "pgscript/objects/pgsRecord.h"

#include "pgscript/objects/pgsVariable.h"

pgsLowerEqual::pgsLowerEqual(const pgsExpression * left, const pgsExpression * right) :
	pgsOperation(left, right)
{
	
}

pgsLowerEqual::~pgsLowerEqual()
{
	
}

pgsExpression * pgsLowerEqual::clone() const
{
	return pnew pgsLowerEqual(*this);
}

pgsLowerEqual::pgsLowerEqual(const pgsLowerEqual & that) :
	pgsOperation(that)
{

}

pgsLowerEqual & pgsLowerEqual::operator =(const pgsLowerEqual & that)
{
	if (this != &that)
	{
		pgsOperation::operator=(that);
	}
	return (*this);
}

wxString pgsLowerEqual::value() const
{
	return wxString() << m_left->value() << wxT(" <= ") << m_right->value();
}

pgsOperand pgsLowerEqual::eval(pgsVarMap & vars) const
{
	// Evaluate operands
	pgsOperand left(m_left->eval(vars));
	pgsOperand right(m_right->eval(vars));
	
	// Return the result
	return (*left <= *right);
}
