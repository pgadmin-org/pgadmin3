//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// ctlSQLBox.h - SQL syntax highlighting textbox
//
//////////////////////////////////////////////////////////////////////////

#ifndef CTLSQLBOX_H
#define CTLSQLBOX_H

// wxWindows headers
#include <wx/wx.h>
#include <wx/stc/stc.h>

// App headers
#include "../../pgAdmin3.h"

#define SQL_KEYWORDS wxT("ALTER COMMENT CREATE DELETE DROP EXPLAIN GRANT INSERT REVOKE \
                          SELECT UPDATE VACUUM AGGREGATE CONSTRAINT DATABASE FUNCTION GROUP INDEX \
                          LANGUAGE OPERATOR RULE SEQUENCE TABLE TRIGGER ABORT BEGIN \
                          CHECKPOINT CLOSE CLUSTER COMMIT COPY DECLARE FETCH LISTEN \
                          LOAD LOCK MOVE NOTIFY REINDEX RESET ROLLBACK SET SHOW TRUNCATE \
                          UNLISTEN AND AS ASC ASCENDING BY CASE DESC DESCENDING ELSE FROM END HAVING INTO \
                          ON OR ORDER THEN USING WHEN WHERE")

// Class declarations
class ctlSQLBox : public wxStyledTextCtrl
{
public:
    ctlSQLBox(wxWindow *parent = (wxWindow *) NULL, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
    ~ctlSQLBox();
};

#endif