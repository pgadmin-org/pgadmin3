//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/statements/pgsStmtList.h"

#include <typeinfo>
#include "pgscript/exceptions/pgsBreakException.h"
#include "pgscript/exceptions/pgsContinueException.h"
#include "pgscript/exceptions/pgsInterruptException.h"
#include "pgscript/utilities/pgsThread.h"
#include "pgscript/utilities/pgsUtilities.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(pgsListStmt);

bool pgsStmtList::m_exception_thrown = false;

pgsStmtList::pgsStmtList(pgsOutputStream &cout, pgsThread *app) :
	pgsStmt(app), m_cout(cout)
{

}

pgsStmtList::~pgsStmtList()
{
	pgsListStmt::iterator it;
	for (it = m_stmt_list.begin(); it != m_stmt_list.end(); it++)
	{
		pdelete(*it);
	}
}

void pgsStmtList::eval(pgsVarMap &vars) const
{
	pgsListStmt::const_iterator it;
	for (it = m_stmt_list.begin(); it != m_stmt_list.end(); it++)
	{
		pgsStmt *current = *it;

		try
		{
			current->eval(vars);

			if (m_app != 0 && m_app->TestDestroy())
				throw pgsInterruptException();
		}
		catch (const pgsException &e)
		{
			if (!m_exception_thrown && (typeid(e) != typeid(pgsBreakException))
			        && (typeid(e) != typeid(pgsContinueException)))
			{
				if (m_app != 0)
				{
					m_app->LockOutput();
					m_app->last_error_line(current->line());
				}

				m_cout << wx_static_cast(const wxString, e.message())
				       << wxT(" on line ") << current->line() << wxT("\n");
				m_exception_thrown = true;

				if (m_app != 0)
				{
					m_app->UnlockOutput();
				}
			}
			throw;
		}
		catch (const std::exception &e)
		{
			if (!m_exception_thrown)
			{
				if (m_app != 0)
				{
					m_app->LockOutput();
					m_app->last_error_line(current->line());
				}

				m_cout << PGSOUTERROR << _("Unknown exception:\n")
				       << wx_static_cast(const wxString,
				                         wxString(e.what(), wxConvUTF8));
				m_exception_thrown = true;

				if (m_app != 0)
				{
					m_app->UnlockOutput();
				}
			}
			throw;
		}

		if (m_app != 0)
		{
			m_app->Yield();
		}
	}
}

void pgsStmtList::insert_front(pgsStmt *stmt)
{
	m_stmt_list.push_front(stmt);
}

void pgsStmtList::insert_back(pgsStmt *stmt)
{
	m_stmt_list.push_back(stmt);
}
