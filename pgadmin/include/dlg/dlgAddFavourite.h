//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2009, The pgAdmin Development Team
// This software is released under the BSD Licence
//
// dlgAddFavourite.h - Add a favourite
//
//////////////////////////////////////////////////////////////////////////

#ifndef dlgAddFavourite_H
#define dlgAddFavourite_H

#include "dlg/dlgClasses.h"
#include "utils/favourites.h"

// Class declarations
class dlgAddFavourite : public pgDialog
{
public:
    dlgAddFavourite(wxWindow *parent, queryFavouriteFolder *favourites);
    ~dlgAddFavourite();
	bool AddFavourite(wxString newtext);
    
private:
	queryFavouriteFolder *favourites;
    void OnOK(wxCommandEvent& ev);
    void OnCancel(wxCommandEvent& ev);
	void OnChange(wxCommandEvent &ev);
	void OnTreeChange(wxTreeEvent &ev);
    DECLARE_EVENT_TABLE()
};

#endif
