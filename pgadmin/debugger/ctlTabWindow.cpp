//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ctlTabWindow.cpp - debugger
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"

// wxWindows headers
#include <wx/wx.h>
#include <wx/thread.h>

// App headers
#include "ctl/ctlAuiNotebook.h"
#include "debugger/ctlTabWindow.h"
#include "debugger/dbgConst.h"


IMPLEMENT_CLASS(ctlTabWindow, wxWindow)

////////////////////////////////////////////////////////////////////////////////
// ctlTabWindow constructor
//
//    This constructor creates a new notebook (a tab control) and clears out the
//  rest of the data members.
//
ctlTabWindow::ctlTabWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos,
                           const wxSize &size, long style, const wxString &name)
	: ctlAuiNotebook(parent, id, pos, size, style),
	  m_resultWindow(NULL),
	  m_varWindow(NULL),
	  m_pkgVarWindow(NULL),
	  m_stackWindow(NULL),
	  m_paramWindow(NULL),
	  m_messageWindow(NULL)
{
	SetFont(settings->GetSystemFont());
	m_tabMap   = new wsTabHash();
}

void ctlTabWindow::SelectTab(wxWindowID id)
{
	wsTabHash::iterator result = m_tabMap->find(id);

	if (result != m_tabMap->end())
	{
		SetSelection(result->second);
	}
}

////////////////////////////////////////////////////////////////////////////////
// GetResultWindow()
//
//    This function returns a pointer to our child result window (m_resultWindow)
//  and creates that window when we first need it.
//
ctlResultGrid *ctlTabWindow::GetResultWindow()
{
	if (m_resultWindow == 0)
	{
		// We don't have a result window yet - go ahead and create one

		m_resultWindow = new ctlResultGrid(this, -1);
		AddPage(m_resultWindow, _("Results"), true);
	}

	return m_resultWindow;
}

////////////////////////////////////////////////////////////////////////////////
// GetVarWindow()
//
//    This function returns a pointer to our child 'local-variables' window
//  (m_varWindow) and creates that window when we first need it.
//
ctlVarWindow *ctlTabWindow::GetVarWindow(bool _create)
{
	if ((m_varWindow == NULL) && _create)
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_VARGRID] = GetPageCount();

		m_varWindow = new ctlVarWindow(this, ID_VARGRID);
		AddPage(m_varWindow, _("Local Variables"), true);
	}

	return m_varWindow;
}

////////////////////////////////////////////////////////////////////////////////
// GetPkgVarWindow()
//
//  This function returns a pointer to our child 'package-variables' window
//  (m_varWindow) and creates that window when we first need it.
//
ctlVarWindow *ctlTabWindow::GetPkgVarWindow(bool create)
{
	if ((m_pkgVarWindow == NULL) && create)
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_PKGVARGRID] = GetPageCount();

		m_pkgVarWindow = new ctlVarWindow(this, ID_PKGVARGRID);
		AddPage(m_pkgVarWindow, _("Package Variables"), true);
	}

	return m_pkgVarWindow;
}

////////////////////////////////////////////////////////////////////////////////
// GetParamWindow()
//
//    This function returns a pointer to our child 'parameters' window
//  (m_paramWindow) and creates that window when we first need it.
//
ctlVarWindow *ctlTabWindow::GetParamWindow(bool create)
{
	if ((m_paramWindow == NULL) && create)
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_PARAMGRID] = GetPageCount();

		m_paramWindow = new ctlVarWindow(this, ID_PARAMGRID);
		AddPage(m_paramWindow, _("Parameters"), true);
	}

	return m_paramWindow;
}

////////////////////////////////////////////////////////////////////////////////
// GetMessageWindow()
//
//    This function returns a pointer to our child 'messages' window
//  (m_messageWindow) and creates that window when we first need it.
//
ctlMessageWindow *ctlTabWindow::GetMessageWindow()
{
	if (m_messageWindow == 0)
	{
		// We don't have a variable window yet - go ahead and create one

		(*m_tabMap)[ID_MSG_PAGE] = GetPageCount();

		m_messageWindow = new ctlMessageWindow(this, ID_MSG_PAGE);
		AddPage(m_messageWindow, _("DBMS Messages"), true);
	}
	return m_messageWindow;
}

////////////////////////////////////////////////////////////////////////////////
// GetStackWindow()
//
//    This function returns a pointer to our child stack-trace window
//  (m_stackWindow) and creates that window when we first need it.
//
ctlStackWindow *ctlTabWindow::GetStackWindow()
{
	if (m_stackWindow == 0)
	{
		// We don't have a stack-trace window yet - go ahead and create one
		m_stackWindow = new ctlStackWindow(this, -1);
		AddPage(m_stackWindow, _("Stack"), true);
	}

	return m_stackWindow;
}

