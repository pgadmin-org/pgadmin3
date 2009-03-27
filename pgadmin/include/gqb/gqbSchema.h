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
    void createObjects(gqbBrowser *tablesBrowser,  pgConn *conn, OID oidVal, wxTreeItemId parentNode, int tableImage, int viewImage, int xTableImage);

private:
    pgConn *conn;
    wxString NumToStr(OID value);
    void createTables(pgConn *conn, gqbBrowser *tablesBrowser, wxTreeItemId parentNode, OID oidVal, int tableImage, int viewImage, int xTableImage);
};
#endif
