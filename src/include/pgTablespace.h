//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2004, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgTablespace.h - PostgreSQL Tablespace
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGTABLESPACE_H
#define PGTABLESPACE_H


class pgTablespace : public pgServerObject
{
public:
    pgTablespace(const wxString& newName = wxT(""));
    ~pgTablespace();

    int GetIcon() { return PGICON_TABLESPACE; }

    void ShowTreeDetail(wxTreeCtrl *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);
    static pgObject *ReadObjects(pgCollection *collection, wxTreeCtrl *browser, const wxString &restriction=wxT(""));
    static void ShowStatistics(pgCollection *collection, ctlListView *statistics);
    void ShowReferencedBy(ctlListView *referencedBy, const wxString &where=wxEmptyString);
    
    wxString GetLocation() const { return location; };
    void iSetLocation(const wxString& newVal) { location = newVal; }
    
    bool DropObject(wxFrame *frame, wxTreeCtrl *browser);
    bool RequireDropConfirm() { return true; }
    pgConn *connection();

    wxString GetSql(wxTreeCtrl *browser);
    pgObject *Refresh(wxTreeCtrl *browser, const wxTreeItemId item);

private:
    wxString location;
};


#endif
