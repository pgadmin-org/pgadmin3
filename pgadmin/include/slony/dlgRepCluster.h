//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgRepCluster.h - Slony-I cluster property
//
//////////////////////////////////////////////////////////////////////////


#ifndef __DLG_REPCLUSTERPROP
#define __DLG_REPCLUSTERPROP

#include "dlg/dlgProperty.h"


class slCluster;
class sysProcess;
class wxProcessEvent;

class dlgRepClusterBase : public dlgProperty
{
public:
	dlgRepClusterBase(pgaFactory *factory, frmMain *frame, const wxString &dlgName, slCluster *cl, pgDatabase *obj);
	~dlgRepClusterBase();
	pgObject *GetObject();
	int Go(bool modal);

private:
	virtual void OnChangeCluster(wxCommandEvent &ev) = 0;

protected:
	void OnChangeServer(wxCommandEvent &ev);
	void OnChangeDatabase(wxCommandEvent &ev);

	bool AddScript(wxString &sql, const wxString &filename);
	slCluster *cluster;
	wxTreeItemId servers;
	pgServer *remoteServer;
	pgConn *remoteConn;
	wxString remoteVersion;
	wxString createScript;

	DECLARE_EVENT_TABLE()
};


class dlgRepCluster : public dlgRepClusterBase
{
public:
	dlgRepCluster(pgaFactory *factory, frmMain *frame, slCluster *cl, pgDatabase *obj);
	int Go(bool modal);
	wxString GetHelpPage() const;

	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);

private:
	void OnOK(wxCommandEvent &ev);
	void OnChangeJoin(wxCommandEvent &ev);
	void OnChangeCluster(wxCommandEvent &ev);
	void OnEndProcess(wxProcessEvent &event);

	bool CopyTable(pgConn *from, pgConn *to, const wxString &table);
	sysProcess *process;

	bool SlonyMaximumVersion(const wxString &series, long minor);

	wxArrayLong usedNodes;
	wxString clusterBackup;
	wxString slonyVersion;


	DECLARE_EVENT_TABLE()
};



class dlgRepClusterUpgrade : public dlgRepClusterBase
{
public:
	dlgRepClusterUpgrade(pgaFactory *factory, frmMain *frame, slCluster *cl);
	int Go(bool modal);
	wxString GetHelpPage() const
	{
		return wxT("slony-install#upgrade");
	}


	void CheckChange();
	wxString GetSql();
	pgObject *CreateObject(pgCollection *collection);

private:
	void OnChangeCluster(wxCommandEvent &ev);

	wxString version;
	wxString sql;
	DECLARE_EVENT_TABLE()
};

#endif
