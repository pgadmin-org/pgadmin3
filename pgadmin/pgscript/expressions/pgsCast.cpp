//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsCast.h"

#include "pgscript/exceptions/pgsParameterException.h"
#include "pgscript/objects/pgsNumber.h"

#include "pgscript/parser.tab.hh"
typedef pgscript::pgsParser::token token;

pgsCast::pgsCast(const int &cast_type, const pgsExpression *var) :
	pgsExpression(), m_cast_type(cast_type), m_var(var)
{

}

pgsCast::~pgsCast()
{
	pdelete(m_var);
}

pgsExpression *pgsCast::clone() const
{
	return pnew pgsCast(*this);
}

pgsCast::pgsCast(const pgsCast &that) :
	pgsExpression(that), m_cast_type(that.m_cast_type)
{
	m_var = that.m_var->clone();
}

pgsCast &pgsCast::operator =(const pgsCast &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		m_cast_type = that.m_cast_type;
		pdelete(m_var);
		m_var = that.m_var->clone();
	}
	return (*this);
}

wxString pgsCast::value() const
{
	wxString cast_type;

	switch (m_cast_type)
	{
		case token::PGS_INTEGER:
			cast_type = wxT("integer");
			break;
		case token::PGS_REAL:
			cast_type = wxT("real");
			break;
		case token::PGS_RECORD:
			cast_type = wxT("record");
			break;
		case token::PGS_STRING:
			cast_type = wxT("string");
			break;
		default:
			cast_type = wxT("unknown");
			break;
	}
	return wxString() << wxT("CAST (") << m_var->value() << wxT(" AS ")
	       << cast_type.Upper() << wxT(") ");
}

pgsOperand pgsCast::eval(pgsVarMap &vars) const
{
	pgsOperand var = m_var->eval(vars);
	MAPM num;

	switch (m_cast_type)
	{
		case token::PGS_INTEGER:
			num = pgsMapm::pgs_str_mapm(var->number().value());
			num = pgsMapm::pgs_mapm_round(num);
			return pnew pgsNumber(pgsMapm::pgs_mapm_str(num, true), pgsInt);
		case token::PGS_REAL:
			return pnew pgsNumber(var->number().value(), pgsReal);
		case token::PGS_RECORD:
			return var->record().clone();
		case token::PGS_STRING:
			return var->string().clone();
		default:
			return var->clone();
	}
}
