//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
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
