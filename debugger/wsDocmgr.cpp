//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsdocmgr.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "debugger.h"
#include "wsDocmgr.h"
#include "wsPgconn.h"
#include <wx/choicdlg.h>
#include <wx/filedlg.h>

wxDocTemplate * wsDocMgr::SelectDocumentPath( wxDocTemplate ** templates, int noTemplates, wxString& path, long flags, bool save )
{
	const wxString choices[] = { _( "Server" ), _( "Local Filesystem" ) };

	wxSingleChoiceDialog dialog( NULL, _( "Please select a data source" ), _( "Open Function" ), WXSIZEOF( choices ), choices );

	dialog.SetSelection( 0 );

	if( dialog.ShowModal() == wxID_OK )
	{
		if( dialog.GetSelection() == 0 )
			path = selectFunctionFromServer();
		else
			path = wxFileSelector( _( "Choose file" ), wxT( "" ), wxT( "" ), wxT( ".sql" ), _( "SQL Files(*.sql)|*.sql" ));

		if( path.empty())
			return( NULL );

		return( FindTemplateForPath( path ));
	}
	else
	{
		return( NULL );
	}
}

wxString wsDocMgr::selectFunctionFromServer()
{
	wsConnProp connectProperties( glApp->getConnProp());

	wsPgConn * conn = new wsPgConn( connectProperties );

	if( conn->isConnected())
	{
		PGresult * result = conn->waitForCommand( wxT( "SELECT * from pg_proc WHERE prolang = ( SELECT oid FROM pg_language WHERE lanname = 'plpgsql' )" ));

		if( PQresultStatus( result ) == PGRES_TUPLES_OK )
		{
			int	colCount = PQnfields( result );
			int rowCount = PQntuples( result );

			wxArrayString	choices;

			choices.Alloc( rowCount );

			for( int i = 0; i < PQntuples( result ); ++i )
			{
				choices.Add( wxString( PQgetvalue( result, i, 0 ), wxConvUTF8 ));
			}
			
			wxSingleChoiceDialog dialog( NULL, _( "Please select a function" ), _( "Choose Function" ), choices );

			if( dialog.ShowModal() == wxID_OK )
			{
				wxString result( wxT( "::" ));

				result += dialog.GetStringSelection();
				result += wxT( ".sql" );

				return( result );
			}
		}
		else
		{
			wxMessageBox( wxString( PQresultErrorMessage( result ), wxConvUTF8 ), _( "Can't read function list from server" ), wxOK | wxICON_EXCLAMATION );
			return(wxEmptyString);
		}
	}
    return wxEmptyString;
}
