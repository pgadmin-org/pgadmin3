//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: frmQuery.cpp 7381 2008-08-11 11:33:42Z dpage $
// Copyright (C) 2002 - 2008, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// gqbBrowser.h - Tables Tree of GQB.
//
//////////////////////////////////////////////////////////////////////////

#ifndef GQBBROWSER_H
#define GQBBROWSER_H

// typedef unsigned long OID;

class gqbController;

class gqbBrowser : public wxTreeCtrl
{
public:
    gqbBrowser(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, gqbController *_controller);
    ~gqbBrowser();
    wxTreeItemId& createRoot(wxString &Name);
    wxTreeItemId& getCatalogRootNode(){return catalogsNode;}
    wxTreeItemId& getTablesRootNode(){return schemasNode;}
    void refreshTables(pgConn *connection);
    void setDnDPoint(int x, int y){xx=x; yy=y;};

private:
    enum typeSchema						// GQB-TODO: DELETE from here should be locate at gqbDatabase
    {
        GQB_CATALOG,
        GQB_OTHER
    };

    wxTreeItemId rootNode,catalogsNode,schemasNode;
    void OnItemActivated(wxTreeEvent& event);
    void OnBeginDrag(wxTreeEvent& event);
    wxString NumToStr(OID value);
    gqbController *controller;			//Allow access to controller functions like add table to model
    wxImageList* imageList;
    int xx,yy;

    DECLARE_EVENT_TABLE()
};
#endif
