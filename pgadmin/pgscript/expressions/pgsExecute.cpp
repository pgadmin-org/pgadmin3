//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"
#include "pgscript/expressions/pgsExecute.h"

#include <wx/regex.h>
#include "db/pgConn.h"
#include "db/pgQueryThread.h"
#include "pgscript/objects/pgsNumber.h"
#include "pgscript/objects/pgsRecord.h"
#include "pgscript/objects/pgsString.h"
#include "pgscript/utilities/pgsUtilities.h"
#include "pgscript/utilities/pgsThread.h"

pgsExecute::pgsExecute(const wxString &query, pgsOutputStream *cout,
                       pgsThread *app) :
	pgsExpression(), m_query(query), m_cout(cout), m_app(app)
{

}

pgsExecute::~pgsExecute()
{

}

pgsExpression *pgsExecute::clone() const
{
	return pnew pgsExecute(*this);
}

pgsExecute &pgsExecute::operator=(const pgsExecute &that)
{
	if (this != &that)
	{
		pgsExpression::operator=(that);
		m_query = that.m_query;
		m_app = that.m_app;
		m_query = that.m_query;
	}
	return (*this);
}

wxString pgsExecute::value() const
{
	return m_query;
}

pgsOperand pgsExecute::eval(pgsVarMap &vars) const
{
	// Copy statement locally
	wxString stmt(m_query);

	// Build regular expressions
	wxRegEx identifier(wxT("([^\\])(@[a-zA-Z0-9_#@]+)"));
	wxRegEx escaped(wxT("\\\\(@|\\\\)")); // Backslash followed by @ or backslash
	wxASSERT(identifier.IsValid() && escaped.IsValid());

	// Replace variables in statement
	while (identifier.Matches(stmt))
	{
		wxString var = identifier.GetMatch(stmt, 2);
		wxString chr = identifier.GetMatch(stmt, 1);
		if (vars.find(var) != vars.end())
		{
			wxString res = vars[var]->eval(vars)->value();
			identifier.ReplaceFirst(&stmt, chr + pgsUtilities::escape_quotes(res));
		}
		else
		{
			identifier.ReplaceFirst(&stmt, chr + wxT("\\\\") + var);
		}
	}
	escaped.ReplaceAll(&stmt, wxT("\\1"));

	// Perform operations only if we have a valid connection
	if (m_app != 0 && m_app->connection() != 0 && !m_app->TestDestroy())
	{
		pgQueryThread thread(m_app->connection(), stmt);

		if (thread.Create() == wxTHREAD_NO_ERROR)
		{
			if (thread.Run() == wxTHREAD_NO_ERROR)
			{
				while (true)
				{
					if (m_app->TestDestroy()) // wxThread::TestDestroy()
					{
						thread.Delete();
						break;
					}
					else if (thread.IsRunning())
					{
						m_app->Yield();
						m_app->Sleep(20);
					}
					else
					{
						thread.Wait();
						break;
					}
				}

				if (thread.ReturnCode() != PGRES_COMMAND_OK
				        && thread.ReturnCode() != PGRES_TUPLES_OK)
				{
					if (m_cout != 0)
					{
						m_app->LockOutput();

						(*m_cout) << PGSOUTWARNING;
						wxString message(stmt + wxT("\n") + thread
						                 .GetMessagesAndClear().Strip(wxString::both));
						wxRegEx multilf(wxT("(\n)+"));
						multilf.ReplaceAll(&message, wxT("\n"));
						message.Replace(wxT("\n"), wxT("\n")
						                + generate_spaces(PGSOUTWARNING.Length()));
						(*m_cout) << message << wxT("\n");

						m_app->UnlockOutput();
					}
				}
				else if (!m_app->TestDestroy())
				{
					if (m_cout != 0)
					{
						m_app->LockOutput();

						(*m_cout) << PGSOUTQUERY;
						wxString message(thread.GetMessagesAndClear()
						                 .Strip(wxString::both));
						if (!message.IsEmpty())
							message = stmt + wxT("\n") + message;
						else
							message = stmt;
						wxRegEx multilf(wxT("(\n)+"));
						multilf.ReplaceAll(&message, wxT("\n"));
						message.Replace(wxT("\n"), wxT("\n")
						                + generate_spaces(PGSOUTQUERY.Length()));
						(*m_cout) << message << wxT("\n");

						m_app->UnlockOutput();
					}

					pgsRecord *rec = 0;

					if (thread.DataValid())
					{
						pgSet *set = thread.DataSet();
						set->MoveFirst();
						rec = pnew pgsRecord(set->NumCols());
						wxArrayLong columns_int; // List of columns that contain integers
						wxArrayLong columns_real; // List of columns that contain reals
						for (long i = 0; i < set->NumCols(); i++)
						{
							rec->set_column_name(i, set->ColName(i));
							wxString col_type = set->ColType(i);
							if (!col_type.CmpNoCase(wxT("bigint"))
							        || !col_type.CmpNoCase(wxT("smallint"))
							        || !col_type.CmpNoCase(wxT("integer")))
							{
								columns_int.Add(i);
							}
							else if (!col_type.CmpNoCase(wxT("real"))
							         || !col_type.CmpNoCase(wxT("double precision"))
							         || !col_type.CmpNoCase(wxT("money"))
							         || !col_type.CmpNoCase(wxT("numeric")))
							{
								columns_real.Add(i);
							}
						}
						size_t line = 0;
						while (!set->Eof())
						{
							for (long i = 0; i < set->NumCols(); i++)
							{
								wxString value = set->GetVal(i);

								if (columns_int.Index(i) != wxNOT_FOUND
								        && pgsNumber::num_type(value) == pgsNumber::pgsTInt)
								{
									rec->insert(line, i, pnew pgsNumber(value, pgsInt));
								}
								else if (columns_real.Index(i) != wxNOT_FOUND
								         && pgsNumber::num_type(value) == pgsNumber::pgsTReal)
								{
									rec->insert(line, i, pnew pgsNumber(value, pgsReal));
								}
								else
								{
									rec->insert(line, i, pnew pgsString(value));
								}
							}
							set->MoveNext();
							++line;
						}
					}
					else
					{
						rec = pnew pgsRecord(1);
						rec->insert(0, 0, pnew pgsNumber(wxT("1")));
					}

					return rec;
				}
			}
			else
			{
				wxLogError(wxT("PGSCRIPT: Cannot run query thread for the query:\n%s"),
				           m_query.c_str());
			}
		}
		else
		{
			wxLogError(wxT("PGSCRIPT: Cannot create query thread for the query:\n%s"),
			           m_query.c_str());
		}
	}

	// This must return a record whatever happens
	return pnew pgsRecord(1);
}
