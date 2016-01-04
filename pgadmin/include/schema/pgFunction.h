//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgFunction.h PostgreSQL Function
//
//////////////////////////////////////////////////////////////////////////

#ifndef PGFUNCTION_H
#define PGFUNCTION_H

#include "pgSchema.h"

class pgFunction;

class pgFunctionFactory : public pgSchemaObjFactory
{
public:
	pgFunctionFactory(const wxChar *tn = 0, const wxChar *ns = 0, const wxChar *nls = 0, wxImage *img = 0);
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
	virtual pgCollection *CreateCollection(pgObject *obj);

	pgFunction *AppendFunctions(pgObject *obj, pgSchema *schema, ctlTree *browser, const wxString &restriction);
};


extern pgFunctionFactory functionFactory;

class pgFunction : public pgSchemaObject
{
public:
	pgFunction(pgSchema *newSchema, const wxString &newName = wxT(""));
	pgFunction(pgSchema *newSchema, pgaFactory &factory, const wxString &newName = wxT(""));

	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowTreeDetail(ctlTree *browser, frmMain *form = 0, ctlListView *properties = 0, ctlSQLBox *sqlPane = 0);
	bool CanDropCascaded()
	{
		return GetSchema()->GetMetaType() != PGM_CATALOG;
	}
	void ShowStatistics(frmMain *form, ctlListView *statistics);

	virtual bool GetIsProcedure() const
	{
		return false;
	}

	wxString GetFullName();
	wxString GetArgListWithNames(bool multiline = false);
	wxString GetArgSigList(const bool forScript = false);

	wxArrayString &GetArgNamesArray()
	{
		return argNamesArray;
	}
	void iAddArgName(const wxString &s)
	{
		argNamesArray.Add(s);
	}
	wxArrayString &GetArgTypesArray()
	{
		return argTypesArray;
	}
	void iAddArgType(const wxString &s)
	{
		argTypesArray.Add(s);
	}
	wxArrayString &GetArgModesArray()
	{
		return argModesArray;
	}
	void iAddArgMode(const wxString &s)
	{
		argModesArray.Add(s);
	}
	wxArrayString &GetArgDefsArray()
	{
		return argDefsArray;
	}
	void iAddArgDef(const wxString &s)
	{
		argDefsArray.Add(s);
	}

	wxString GetReturnType() const
	{
		return returnType;
	}
	void iSetReturnType(const wxString &s)
	{
		returnType = s;
	}
	wxString GetLanguage() const
	{
		return language;
	}
	void iSetLanguage(const wxString &s)
	{
		language = s;
	}
	wxString GetVolatility() const
	{
		return volatility;
	}
	void iSetVolatility(const wxString &s)
	{
		volatility = s;
	}
	wxString GetSource() const
	{
		return source;
	}
	void iSetSource(const wxString &s)
	{
		source = s;
	}
	wxString GetBin() const
	{
		return bin;
	}
	void iSetBin(const wxString &s)
	{
		bin = s;
	}
	long GetArgCount() const
	{
		return argCount;
	}
	void iSetArgCount(long ac)
	{
		argCount = ac;
	}
	void iSetArgDefValCount(long ac)
	{
		argDefValCount = ac;
	}
	long GetArgDefValCount()
	{
		return argDefValCount;
	}
	long GetCost() const
	{
		return cost;
	}
	void iSetCost(long c)
	{
		cost = c;
	}
	long GetRows() const
	{
		return rows;
	}
	void iSetRows(long r)
	{
		rows = r;
	}
	bool GetReturnAsSet() const
	{
		return returnAsSet;
	}
	void iSetReturnAsSet(bool b)
	{
		returnAsSet = b;
	}
	bool GetSecureDefiner() const
	{
		return secureDefiner;
	}
	void iSetSecureDefiner(bool b)
	{
		secureDefiner = b;
	}
	bool GetIsStrict() const
	{
		return isStrict;
	}
	void iSetIsStrict(bool b)
	{
		isStrict = b;
	}
	bool GetIsWindow() const
	{
		return isWindow;
	}
	void iSetIsWindow(bool b)
	{
		isWindow = b;
	}
	long GetProcType()
	{
		return procType;
	}
	void iSetProcType(long l)
	{
		procType = l;
	}
	wxArrayString &GetConfigList()
	{
		return configList;
	}
	bool GetIsLeakProof() const
	{
		return isLeakProof;
	}
	void iSetIsLeakProof(bool b)
	{
		isLeakProof = b;
	}

	bool CanRestore()
	{
		return true;
	}
	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetSql(ctlTree *browser);
	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createfunction");
	}
	pgObject *Refresh(ctlTree *browser, const wxTreeItemId item);
	wxString GetSelectSql(ctlTree *browser);

	bool ResetStats();

	void ShowHint(frmMain *form, bool force);
	bool GetCanHint()
	{
		return true;
	};

	bool HasStats()
	{
		return false;
	}
	bool HasDepends()
	{
		return true;
	}
	bool HasReferences()
	{
		return true;
	}

	bool IsUpToDate();

protected:
	pgFunction(pgSchema *newSchema, int newType, const wxString &newName = wxT(""));

private:
	wxString returnType, language, volatility, source, bin;
	wxArrayString argNamesArray, argTypesArray, argModesArray, argDefsArray;
	bool returnAsSet, secureDefiner, isStrict, isWindow, isLeakProof;
	long argCount, cost, rows, argDefValCount, procType;
	wxArrayString configList;
};


class pgFunctionCollection : public pgSchemaObjCollection
{
public:
	pgFunctionCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
	void ShowStatistics(frmMain *form, ctlListView *statistics);
};


class pgTriggerFunctionFactory : public pgFunctionFactory
{
public:
	pgTriggerFunctionFactory();
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
};
extern pgTriggerFunctionFactory triggerFunctionFactory;


class pgTriggerFunction : public pgFunction
{
public:
	pgTriggerFunction(pgSchema *newSchema, const wxString &newName = wxT(""));
	wxString GetTranslatedMessage(int kindOfMessage) const;
	static pgObject *ReadObjects(pgCollection *collection, ctlTree *browser);
};


class pgTriggerFunctionCollection : public pgSchemaObjCollection
{
public:
	pgTriggerFunctionCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};


class pgProcedureFactory : public pgFunctionFactory
{
public:
	pgProcedureFactory();
	virtual dlgProperty *CreateDialog(frmMain *frame, pgObject *node, pgObject *parent);
	virtual pgObject *CreateObjects(pgCollection *obj, ctlTree *browser, const wxString &restr = wxEmptyString);
};
extern pgProcedureFactory procedureFactory;


class pgProcedure : public pgFunction
{
public:
	pgProcedure(pgSchema *newSchema, const wxString &newName = wxT(""));
	wxString GetFullName();
	wxString GetTranslatedMessage(int kindOfMessage) const;
	static pgObject *ReadObjects(pgCollection *collection, ctlTree *browser);

	bool GetIsProcedure() const
	{
		return true;
	}

	wxString GetSql(ctlTree *browser);
	bool DropObject(wxFrame *frame, ctlTree *browser, bool cascaded);
	wxString GetExecSql(ctlTree *browser);

	wxString GetHelpPage(bool forCreate) const
	{
		return wxT("pg/sql-createprocedure");
	}
};


class resetFunctionStatsFactory : public contextActionFactory
{
public:
	resetFunctionStatsFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar);
	wxWindow *StartDialog(frmMain *form, pgObject *obj);
	bool CheckEnable(pgObject *obj);
};


class pgProcedureCollection : public pgSchemaObjCollection
{
public:
	pgProcedureCollection(pgaFactory *factory, pgSchema *sch);
	wxString GetTranslatedMessage(int kindOfMessage) const;
};


#endif
