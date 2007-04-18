//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsconnprop.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "wsConnProp.h"
#include "debugger.h"

#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>

////////////////////////////////////////////////////////////////////////////////
// wsConnPropDlg constructor
//
//	This function creates a dialog that prompts the user for connection 
//  properties.  The dialog box that we present here is similar to that 
//  used by pgAdmin3.
//
//  Note that we use an XRC resource file to build the user interface.

wsConnPropDlg::wsConnPropDlg( wxWindow * parent, wxWindowID id )
{
	// NOTE: Don't call the wxDialog constructor here - we need two-stage
	//       construction so we can call the LoadDialog() function. 
    
	wxXmlResource::Get()->InitAllHandlers();
    
    wxWindowBase::SetFont(glApp->GetSystemFont());

	wxXmlResource::Get()->Load( wxT( "resources/wsconnprop.xrc" ));
	wxXmlResource::Get()->LoadDialog( this, parent, wxT( "wsConnPropDlg" ));
    
}

////////////////////////////////////////////////////////////////////////////////
// setDefaults()
//
//	This function copies the default connection properties from the given 
//  wsConnProp object into the edit controls displayed in this dialog.

void wsConnPropDlg::setDefaults( const wsConnProp & defaults )
{
	XRCCTRL( *this, "m_host", wxTextCtrl )->SetValue( defaults.m_host );
	XRCCTRL( *this, "m_database", wxTextCtrl )->SetValue( defaults.m_database );
	XRCCTRL( *this, "m_userName", wxTextCtrl )->SetValue( defaults.m_userName );
	XRCCTRL( *this, "m_password", wxTextCtrl )->SetValue( defaults.m_password );
	XRCCTRL( *this, "m_port",     wxTextCtrl )->SetValue( defaults.m_port );
}

////////////////////////////////////////////////////////////////////////////////
// getChoices()
//
//	This function copies the choices made by the user into the given wsConnProp
//  object (after the user has made any changes).

void wsConnPropDlg::getChoices( wsConnProp & result )
{
	result.m_host     = XRCCTRL( *this, "m_host",     wxTextCtrl )->GetValue();
	result.m_database = XRCCTRL( *this, "m_database", wxTextCtrl )->GetValue();
	result.m_userName = XRCCTRL( *this, "m_userName", wxTextCtrl )->GetValue();
	result.m_password = XRCCTRL( *this, "m_password", wxTextCtrl )->GetValue();
	result.m_port     = XRCCTRL( *this, "m_port",     wxTextCtrl )->GetValue();
}

