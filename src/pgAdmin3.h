//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// Copyright (C) 2002, The pgAdmin Development Team
// This software is released under the pgAdmin Public Licence
//
// pgAdmin3.h - The main application header
//
//////////////////////////////////////////////////////////////////////////

// Application Version
#define VERSION "1.0.0 Devel"

#define SQL_KEYWORDS "ALTER COMMENT CREATE DELETE DROP EXPLAIN GRANT INSERT REVOKE \
                      SELECT UPDATE VACUUM AGGREGATE CONSTRAINT DATABASE FUNCTION GROUP INDEX \
                      LANGUAGE OPERATOR RULE SEQUENCE TABLE TRIGGER ABORT BEGIN \
                      CHECKPOINT CLOSE CLUSTER COMMIT COPY DECLARE FETCH LISTEN \
                      LOAD LOCK MOVE NOTIFY REINDEX RESET ROLLBACK SET SHOW TRUNCATE \
                      UNLISTEN AND AS ASC ASCENDING BY CASE DESC DESCENDING ELSE FROM END HAVING INTO \
                      ON OR ORDER THEN USING WHEN WHERE"

// Classes declarations
class pgAdmin3 : public wxApp
{
public:
  virtual bool OnInit();
};




