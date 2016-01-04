//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/utilities/pgsContext.h"

#include <wx/datetime.h>
#include <wx/regex.h>
#include <typeinfo>
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsString.h"
#include "pgscript/statements/pgsExpressionStmt.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(pgsListExpression);

pgsContext::pgsContext(pgsOutputStream &cout) :
	m_cout(cout)
{

}

pgsContext::~pgsContext()
{

}

pgsVariable *pgsContext::zero()
{
	pgsVariable *zero = pnew pgsNumber(wxT("0"));
	push_var(zero);
	return zero;
}

pgsVariable *pgsContext::one()
{
	pgsVariable *one = pnew pgsNumber(wxT("1"));
	push_var(one);
	return one;
}

pgsVariable *pgsContext::seed()
{
	pgsVariable *seed = pnew pgsNumber(wxString() << wxDateTime::GetTimeNow());
	push_var(seed);
	return seed;
}

pgsVariable *pgsContext::encoding()
{
	pgsVariable *encoding = pnew pgsString(wxLocale::GetSystemEncodingName());
	push_var(encoding);
	return encoding;
}

pgsStmtList *pgsContext::stmt_list(pgsThread *app)
{
	pgsStmtList *stmt_list = pnew pgsStmtList(m_cout, app);
	push_stmt(stmt_list);
	return stmt_list;
}

void pgsContext::add_column(const wxString &column)
{
	m_columns.Add(column);
}

const wxArrayString &pgsContext::columns()
{
	return m_columns;
}

void pgsContext::clear_columns()
{
	m_columns.Clear();
}

void pgsContext::push_var(pgsExpression *var)
{
	wxLogScriptVerbose(wxT("PUSH EXPR %s"), var->value().c_str());
	m_vars.push_back(var);
}

void pgsContext::pop_var()
{
	wxLogScriptVerbose(wxT("POP EXPR %s"), m_vars.back()->value().c_str());
	m_vars.pop_back();
}

size_t pgsContext::size_vars() const
{
	return m_vars.GetCount();
}

void pgsContext::push_stmt(pgsStmt *stmt)
{
	wxLogScriptVerbose(wxT("PUSH STMT %s"), wxString(typeid(*stmt).name(),
	                   wxConvUTF8).c_str());
	m_stmts.push_back(stmt);
}

void pgsContext::pop_stmt()
{
	wxLogScriptVerbose(wxT("POP STMT %s"), wxString(typeid(*(m_stmts.back()))
	                   .name(), wxConvUTF8).c_str());
	m_stmts.pop_back();
}

size_t pgsContext::size_stmts() const
{
	return m_stmts.GetCount();
}

void pgsContext::clear_stacks()
{
	while (!m_vars.empty())
	{
		pdelete(m_vars.back());
		m_vars.pop_back();
	}

	while (!m_stmts.empty())
	{
		pdelete(m_stmts.back());
		m_stmts.pop_back();
	}
}
