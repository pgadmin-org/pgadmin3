//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsConnProp.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class wsConnProp
//
//	A wsConnProp object is used to hold a set of connection properties, that is.
//  it's a collection of all of the information that we need in order to connect
//  to a server.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WSCONNPROPH
#define WSCONNPROPH


#include <wx/string.h>
#include <wx/dialog.h>
class wsConnProp
{

public:
	wxString	m_host;			// Host name (or IP-address)
	wxString	m_database;		// Database name
	wxString	m_userName;		// User name	
	wxString	m_password;		// Password
	wxString	m_port;			// Port number
	wxString	m_debugPort;	// Port number for debugger connection
	int			m_sslMode;		// SSL Mode
};

class wsConnPropDlg : public wxDialog
{
public:
    wsConnPropDlg( wxWindow * parent, wxWindowID id = wxID_ANY );

    void setDefaults( const wsConnProp & defaults );
    void getChoices( wsConnProp & result );
    
private:

    wsConnProp      m_properties;    

};

#endif
