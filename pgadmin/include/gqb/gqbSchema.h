//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmQuery.cpp 7381 2008-08-11 11:33:42Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// gqbSchema.h - Schema object for GQB
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBSCHEMA_H
#define GQBSCHEMA_H

// App headers
#include "gqb/gqbObject.h"
#include "gqb/gqbTable.h"

class gqbSchema : public gqbObject
{
public:
    gqbSchema(gqbObject *parent, wxString name, type_gqbObject type);
    void createObjects(gqbBrowser *_tablesBrowser,  pgConn *_conn, OID oidVal, wxTreeItemId parentNode, int _tableImage, int _viewImage);

private:
    pgConn *conn;
    wxString NumToStr(OID value);
    void createTables(pgConn *conn, gqbBrowser *tablesBrowser, wxTreeItemId parentNode, OID oidVal, int _tableImage, int _viewImage);
};
#endif
