//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id: pgGroup.h 4875 2006-01-06 21:06:46Z dpage $
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// pgGroup.h - PostgreSQL Group
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGGROUP_H
#define PGGROUP_H

#include "pgServer.h"


class pgGroupFactory : public pgServerObjFactory
{
public:
    pgGroupFactory();
    virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
    virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr=wxEmptyString);
};
extern pgGroupFactory groupFactory;


// Class declarations
class pgGroup : public pgServerObject
{
public:
    pgGroup(const wxString& newName = wxT(""));
    ~pgGroup();


    // Group Specific
    long GetGroupId() const { return groupId; }
    void iSetGroupId(const long l) { groupId=l; }
    long GetMemberCount() const { return memberCount; }
    void iSetMemberCount(const long l) { memberCount=l; }
    wxString GetMemberIds() const { return memberIds; }
    void iSetMemberIds(const wxString& s) { memberIds=s; }
    wxString GetMembers() const { return members; }
    void iSetMembers(const wxString& s) { members=s; }
    wxArrayString& GetUsersIn() { return usersIn; }

    void ShowTreeDetail(ctlTree *browser, frmMain *form=0, ctlListView *properties=0, ctlSQLBox *sqlPane=0);

    bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
    wxString GetSql(ctlTree *browser);
    pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);

private:
    long groupId, memberCount;
    wxString memberIds, members, quotedMembers;
    wxArrayString usersIn;
};

#endif
