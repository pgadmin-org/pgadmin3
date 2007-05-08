//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: dbgConnProp.h 6207 2007-04-18 15:14:25Z dpage $
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dbgConnProp.h - debugger 
//
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// class dbgConnProp
//
//  dbgConnProp object is used to hold a set of connection properties, that is.
//  it's a collection of all of the information that we need in order to connect
//  to a server.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DBGCONNPROP_H
#define DBGCONNPROP_H

class dbgConnProp
{

public:
	wxString	m_host;			// Host name (or IP-address)
	wxString	m_database;		// Database name
	wxString	m_userName;		// User name	
	wxString	m_password;		// Password
	wxString	m_port;			// Port number
	wxString	m_debugPort;	// Port number for debugger connection
	int		    m_sslMode;		// SSL Mode
};

#endif
