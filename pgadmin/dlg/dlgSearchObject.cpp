//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2011, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSearchObject.h - Search dialogue
//
//////////////////////////////////////////////////////////////////////////


//AVANT DE LE COMMITER, IL FAUT AJOUTER LA MODIF DU PROJET VS2008

// App headers
#include "pgAdmin3.h"

#include "frm/frmMain.h"
#include "dlg/dlgSearchObject.h"
#include "utils/sysSettings.h"
#include "utils/misc.h"
#include "ctl/ctlListView.h"

#define txtPattern        CTRL_TEXT("txtPattern")
#define cbType          CTRL_COMBOBOX("cbType")
#define lcResults         CTRL_LISTCTRL("lcResults")
#define btnSearch             CTRL_BUTTON("btnSearch")

BEGIN_EVENT_TABLE(dlgSearchObject, pgDialog)
	EVT_BUTTON(wxID_HELP,                      dlgSearchObject::OnHelp)
	EVT_BUTTON(XRCID("btnSearch"),			   dlgSearchObject::OnSearch)
	EVT_BUTTON(wxID_CANCEL,                    dlgSearchObject::OnCancel)
	EVT_TEXT(XRCID("txtPattern"),              dlgSearchObject::OnChange)
	EVT_LIST_ITEM_SELECTED(XRCID("lcResults"), dlgSearchObject::OnSelSearchResult)
END_EVENT_TABLE()

dlgSearchObject::dlgSearchObject(frmMain *p, pgDatabase *db)
{
	parent = p;
	header = wxT("");
	currentdb = db;

	wxWindowBase::SetFont(settings->GetSystemFont());
	LoadResource(p, wxT("dlgSearchObject"));

	// Icon
	appearanceFactory->SetIcons(this);
	RestorePosition();

	btnSearch->Disable();

	lcResults->InsertColumn(0, _("Type"));
	lcResults->InsertColumn(1, _("Name"));
	lcResults->InsertColumn(2, _("Path"));

	// Mapping table between local language and english,
	// because in SQL we're using only english and translate it later
	// to the local language.

	aMap[_("All types")] = wxT("All types");
	aMap[_("Schemas")] = wxT("Schemas");
	aMap[_("Tables")] = wxT("Tables");
	aMap[_("Columns")] = wxT("Columns");
	aMap[_("Triggers")] = wxT("Triggers");
	aMap[_("Views")] = wxT("Views");
	aMap[_("Rules")] = wxT("Rules");
	aMap[_("Indexes")] = wxT("Indexes");
	aMap[_("Functions")] = wxT("Functions");
	aMap[_("Aggregates")] = wxT("Aggregates");
	aMap[_("Trigger Functions")] = wxT("Trigger Functions");
	aMap[_("Constraints")] = wxT("Constraints");
	aMap[_("Sequences")] = wxT("Sequences");
	aMap[_("Types")] = wxT("Types");
	aMap[_("Domains")] = wxT("Domains");
	aMap[_("Languages")] = wxT("Languages");
	aMap[_("Conversions")] = wxT("Conversions");
	aMap[_("Casts")] = wxT("Casts");
	aMap[_("Login Roles")] = wxT("Login Roles");
	aMap[_("Group Roles")] = wxT("Group Roles");
	aMap[_("FTS Configurations")] = wxT("FTS Configurations");
	aMap[_("FTS Dictionarie")] = wxT("FTS Dictionaries");
	aMap[_("FTS Parsers")] = wxT("FTS Parsers");
	aMap[_("FTS Templates")] = wxT("FTS Templates");
	aMap[_("Foreign Data Wrappers")] = wxT("Foreign Data Wrappers");
	aMap[_("Foreign Servers")] = wxT("Foreign Servers");
	aMap[_("Foreign Tables")] = wxT("Foreign Tables");
	aMap[_("User Mappings")] = wxT("User Mappings");
	aMap[_("Operators")] = wxT("Operators");
	aMap[_("Operator Classes")] = wxT("Operator Classes");
	aMap[_("Operator Families")] = wxT("Operator Families");
	aMap[_("Extensions")] = wxT("Extensions");
	aMap[_("Collations")] = wxT("Collations");

	cbType->Clear();
	cbType->Append(_("All types"));
	cbType->Append(_("Schemas"));
	cbType->Append(_("Tables"));
	cbType->Append(_("Columns"));
	cbType->Append(_("Triggers"));
	cbType->Append(_("Views"));
	cbType->Append(_("Rules"));
	cbType->Append(_("Indexes"));
	cbType->Append(_("Functions"));
	cbType->Append(_("Aggregates"));
	cbType->Append(_("Trigger Functions"));
	cbType->Append(_("Constraints"));
	cbType->Append(_("Sequences"));
	cbType->Append(_("Types"));
	cbType->Append(_("Languages"));
	cbType->Append(_("Domains"));
	cbType->Append(_("Conversions"));
	cbType->Append(_("Casts"));
	cbType->Append(_("Login Roles"));
	cbType->Append(_("Group Roles"));
	cbType->Append(_("FTS Configurations"));
	cbType->Append(_("FTS Dictionaries"));
	cbType->Append(_("FTS Parsers"));
	cbType->Append(_("FTS Templates"));
	if(currentdb->BackendMinimumVersion(8, 4))
	{
		cbType->Append(_("Foreign Data Wrappers"));
		cbType->Append(_("Foreign Servers"));
		cbType->Append(_("User Mappings"));
	}
	if(currentdb->BackendMinimumVersion(9, 1))
	{
		cbType->Append(_("Foreign Tables"));
	}
	cbType->Append(_("Operators"));
	cbType->Append(_("Operator Classes"));
	cbType->Append(_("Operator Families"));

	if(currentdb->BackendMinimumVersion(9, 1))
	{
		cbType->Append(_("Extensions"));
		cbType->Append(_("Collations"));
	}
	cbType->SetSelection(0);

	txtPattern->SetFocus();
}


dlgSearchObject::~dlgSearchObject()
{
}

void dlgSearchObject::OnHelp(wxCommandEvent &ev)
{
	DisplayHelp(wxT("searchobject"), HELP_PGADMIN);
}

void dlgSearchObject::OnSelSearchResult(wxListEvent &ev)
{

	long row_number = ev.GetIndex();

	if(lcResults->GetItemTextColour(row_number) == wxColour(128, 128, 128))
	{
		/* Result type is not enabled in settings, so we don't search for it in the tree */
		return;
	}

	//Taken from: http://wiki.wxwidgets.org/WxListCtrl#Get_the_String_Contents_of_a_.22cell.22_in_a_LC_REPORT_wxListCtrl
	wxListItem     row_info;
	wxString       path;

	// Set what row it is (m_itemId is a member of the regular wxListCtrl class)
	row_info.m_itemId = row_number;
	// Set what column of that row we want to query for information.
	row_info.m_col = 2;
	// Set text mask
	row_info.m_mask = wxLIST_MASK_TEXT;

	// Get the info and store it in row_info variable.
	lcResults->GetItem(row_info);

	// Extract the text out that cell
	path = row_info.m_text;


	if(!parent->SetCurrentNode(parent->GetBrowser()->GetRootItem(), path))
	{
		wxMessageBox(_("The specified object couldn't be found in the tree."));
	}

}

void dlgSearchObject::OnChange(wxCommandEvent &ev)
{
	/* When someone searches for operators, the limit of 3 characters is ignored */
	if(aMap[cbType->GetValue()] != wxT("Operators") && txtPattern->GetValue().Length() < 3)
		btnSearch->Disable();
	else
		btnSearch->Enable();
}

void dlgSearchObject::OnSearch(wxCommandEvent &ev)
{

	/*
	Adding objects:

	Create a sql statement which lists all objects of the specified type and add it to the inner statement with an union.
	We need three columns: type, objectname and path.
	Parts of the path which has to be translated to the local langauge (because of tree path) must begin with a colon.
	Append the type to the combobox and the mapping table in the constructor. */

	wxString databasePath = parent->GetNodePath(currentdb->GetDatabase()->GetId());
	wxString searchSQL = wxT("SELECT * FROM (  ")
	                     wxT("	SELECT  ")
	                     wxT("	CASE   ")
	                     wxT("		WHEN c.relkind = 'r' THEN 'Tables'   ")
	                     wxT("		WHEN c.relkind = 'S' THEN 'Sequences'   ")
	                     wxT("		WHEN c.relkind = 'v' THEN 'Views'   ")
	                     wxT("		ELSE 'should not happen'   ")
	                     wxT("	END AS type, c.relname AS objectname,  ")
	                     wxT("	':Schemas/' || n.nspname || '/' ||  ")
	                     wxT("	CASE   ")
	                     wxT("		WHEN c.relkind = 'r' THEN ':Tables'   ")
	                     wxT("		WHEN c.relkind = 'S' THEN ':Sequences'   ")
	                     wxT("		WHEN c.relkind = 'v' THEN ':Views'   ")
	                     wxT("		ELSE 'should not happen'   ")
	                     wxT("	END || '/' || c.relname AS path  ")
	                     wxT("	FROM pg_class c  ")
	                     wxT("	LEFT JOIN pg_namespace n ON n.oid = c.relnamespace     ")
	                     wxT("	WHERE c.relkind in ('r','S','v')  ")
	                     wxT("	UNION  ")
	                     wxT("	SELECT 'Indexes', cls.relname, ':Schemas/' || n.nspname || '/:Tables/' || tab.relname || '/:Indexes/' || cls.relname ")
	                     wxT("	FROM pg_index idx ")
	                     wxT("	JOIN pg_class cls ON cls.oid=indexrelid ")
	                     wxT("	JOIN pg_class tab ON tab.oid=indrelid ")
	                     wxT("	JOIN pg_namespace n ON n.oid=tab.relnamespace ")
	                     wxT("	LEFT JOIN pg_depend dep ON (dep.classid = cls.tableoid AND dep.objid = cls.oid AND dep.refobjsubid = '0' AND dep.refclassid=(SELECT oid FROM pg_class WHERE relname='pg_constraint') AND dep.deptype='i') ")
	                     wxT("	LEFT OUTER JOIN pg_constraint con ON (con.tableoid = dep.refclassid AND con.oid = dep.refobjid) ")
	                     wxT("	LEFT OUTER JOIN pg_description des ON des.objoid=cls.oid ")
	                     wxT("	LEFT OUTER JOIN pg_description desp ON (desp.objoid=con.oid AND desp.objsubid = 0) ")
	                     wxT("	WHERE contype IS NULL ")
	                     wxT("	UNION  ")
	                     wxT("	SELECT CASE WHEN t.typname = 'trigger' THEN 'Trigger Functions' ELSE 'Functions' END AS type, p.proname,  ")
	                     wxT("	':Schemas/' || n.nspname || '/' || case when t.typname = 'trigger' then ':Trigger Functions' else ':Functions' end || '/' || p.proname ")
	                     wxT("	from pg_proc p  ")
	                     wxT("	left join pg_namespace n on p.pronamespace = n.oid  ")
	                     wxT("	left join pg_type t on p.prorettype = t.oid  ")
	                     wxT("	union  ")
	                     wxT("	select 'Schemas', nspname, ':Schemas/' || nspname from pg_namespace  ")
	                     wxT("	union  ")
	                     wxT("	select 'Columns', a.attname,  ")
	                     wxT("	':Schemas/' || n.nspname || '/' ||  ")
	                     wxT("	case   ")
	                     wxT("		when t.relkind = 'r' then ':Tables'   ")
	                     wxT("		when t.relkind = 'S' then ':Sequences'   ")
	                     wxT("		when t.relkind = 'v' then ':Views'   ")
	                     wxT("		else 'should not happen'   ")
	                     wxT("	end || '/' || t.relname || '/:Columns/' || a.attname AS path  ")
	                     wxT("	from pg_attribute a  ")
	                     wxT("	inner join pg_class t on a.attrelid = t.oid and t.relkind in ('r','v')  ")
	                     wxT("	left join pg_namespace n on t.relnamespace = n.oid where a.attnum > 0  ")
	                     wxT("	union  ")
	                     wxT("	select 'Constraints', case when tf.relname is null then c.conname else c.conname || ' -> ' || tf.relname end, ':Schemas/' || n.nspname||'/:Tables/'||t.relname||'/:Constraints/'||case when tf.relname is null then c.conname else c.conname || ' -> ' || tf.relname end from pg_constraint c    ")
	                     wxT("	left join pg_class t on c.conrelid = t.oid  ")
	                     wxT("	left join pg_class tf on c.confrelid = tf.oid  ")
	                     wxT("	left join pg_namespace n on t.relnamespace = n.oid 						 ")
	                     wxT("	union  ")
	                     wxT("	select 'Rules', r.rulename, ':Schemas/' || n.nspname||case when t.relkind = 'v' then '/:Views/' else '/:Tables/' end||t.relname||'/:Rules/'|| r.rulename from pg_rewrite r  ")
	                     wxT("	left join pg_class t on r.ev_class = t.oid  ")
	                     wxT("	left join pg_namespace n on t.relnamespace = n.oid 						 ")
	                     wxT("	union  ")
	                     wxT("	select 'Triggers', tr.tgname, ':Schemas/' || n.nspname||case when t.relkind = 'v' then '/:Views/' else '/:Tables/' end||t.relname || '/:Triggers/' || tr.tgname from pg_trigger tr  ")
	                     wxT("	left join pg_class t on tr.tgrelid = t.oid  ")
	                     wxT("	left join pg_namespace n on t.relnamespace = n.oid  ")
	                     wxT("	where ");
	if(currentdb->BackendMinimumVersion(9, 0))
	{
		searchSQL += wxT(" tr.tgisinternal = false ");
	}
	else
	{
		searchSQL += wxT(" tr.tgisconstraint = false ");
	}
	searchSQL += wxT("	union ")
	             wxT("	SELECT 'Types', t.typname, ':Schemas/' || n.nspname || '/:Types' || t.typname ")
	             wxT("	FROM pg_type t ")
	             wxT("	LEFT OUTER JOIN pg_type e ON e.oid=t.typelem ")
	             wxT("	LEFT OUTER JOIN pg_class ct ON ct.oid=t.typrelid AND ct.relkind <> 'c' ")
	             wxT("	LEFT OUTER JOIN pg_namespace n on t.typnamespace = n.oid ")
	             wxT("	WHERE t.typtype != 'd' AND t.typname NOT LIKE E'\\_%' 	 ")
	             wxT("	union ")
	             wxT("	SELECT 'Conversions', co.conname, ':Schemas/' || n.nspname || '/:Conversions/' || co.conname ")
	             wxT("	FROM pg_conversion co ")
	             wxT("	JOIN pg_namespace n ON n.oid=co.connamespace ")
	             wxT("	LEFT OUTER JOIN pg_description des ON des.objoid=co.oid AND des.objsubid=0	 ")
	             wxT("	union ")
	             wxT("	SELECT 'Casts', format_type(st.oid,NULL) ||'->'|| format_type(tt.oid,tt.typtypmod), ':Casts/' || format_type(st.oid,NULL) ||'->'|| format_type(tt.oid,tt.typtypmod) ")
	             wxT("	FROM pg_cast ca ")
	             wxT("	JOIN pg_type st ON st.oid=castsource ")
	             wxT("	JOIN pg_type tt ON tt.oid=casttarget ")
	             wxT("	union ")
	             wxT("	SELECT 'Languages', lanname, ':Languages/' || lanname ")
	             wxT("	FROM pg_language lan ")
	             wxT("	WHERE lanispl IS TRUE ")
	             wxT("	union ")
	             wxT("	SELECT 'FTS Configurations', cfg.cfgname, ':Schemas/' || n.nspname || '/:FTS Configurations/' || cfg.cfgname ")
	             wxT("	FROM pg_ts_config cfg ")
	             wxT("	left join pg_namespace n on cfg.cfgnamespace = n.oid	 ")
	             wxT("	union ")
	             wxT("	SELECT 'FTS Dictionaries', dict.dictname, ':Schemas/' || ns.nspname || '/:FTS Dictionaries/' || dict.dictname ")
	             wxT("	FROM pg_ts_dict dict ")
	             wxT("	left join pg_namespace ns on dict.dictnamespace = ns.oid ")
	             wxT("	union ")
	             wxT("	SELECT 'FTS Parsers', prs.prsname, ':Schemas/' || ns.nspname || '/:FTS Parsers/' || prs.prsname ")
	             wxT("	FROM pg_ts_parser prs ")
	             wxT("	left join pg_namespace ns on prs.prsnamespace = ns.oid ")
	             wxT("	union ")
	             wxT("	SELECT 'FTS Templates', tmpl.tmplname, ':Schemas/' || ns.nspname || '/:FTS Templates/' || tmpl.tmplname ")
	             wxT("	FROM pg_ts_template tmpl ")
	             wxT("	left join pg_namespace ns on tmpl.tmplnamespace = ns.oid ")
	             wxT("	union ")
	             wxT("	select 'Domains', t.typname, ':Schemas/' || n.nspname || '/:Domains/' || t.typname from pg_type t  ")
	             wxT("	inner join pg_namespace n on t.typnamespace = n.oid ")
	             wxT("	where t.typtype = 'd' ")
	             wxT("	union ")
	             wxT("	select 'Aggregates', pr.proname, ':Schemas/' || ns.nspname || '/:Aggregates/' || pr.proname from pg_catalog.pg_aggregate ag ")
	             wxT("	inner join pg_proc pr on ag.aggfnoid = pr.oid ")
	             wxT("	left join pg_namespace ns on  pr.pronamespace = ns.oid ")
	             wxT("	union ")
	             wxT("	select case when rolcanlogin = true then 'Login Roles' else 'Group Roles' end, rolname, case when rolcanlogin = true then ':Login Roles' else ':Group Roles' end || '/' || rolname ")
	             wxT("	from pg_roles ")
	             wxT("	union ")
	             wxT("	select 'Tablespaces', spcname, ':Tablespaces/'||spcname from pg_tablespace ")
	             wxT("	union ")
	             wxT("	SELECT 'Operators', op.oprname, ':Schemas/' || ns.nspname || '/:Operators/' || op.oprname ")
	             wxT("	FROM pg_operator op ")
	             wxT("	left join pg_namespace ns on op.oprnamespace = ns.oid ")
	             wxT("	union ")
	             wxT("	SELECT 'Operator Classes', op.opcname, ':Schemas/' || ns.nspname || '/:Operator Classes/' || op.opcname ")
	             wxT("	FROM pg_opclass op ")
	             wxT("	left join pg_namespace ns on op.opcnamespace = ns.oid ")
	             wxT("	union ")
	             wxT("	SELECT 'Operator Families', opf.opfname, ':Schemas/' || ns.nspname || '/:Operator Families/' || opf.opfname ")
	             wxT("	FROM pg_opfamily opf ")
	             wxT("	left join pg_namespace ns on opf.opfnamespace = ns.oid ");


	if(currentdb->BackendMinimumVersion(8, 4))
	{
		searchSQL += wxT("	union ")
		             wxT("	select 'Foreign Data Wrappers', fdwname, ':Foreign Data Wrappers/' || fdwname from pg_foreign_data_wrapper ")
		             wxT("	union ")
		             wxT("	select 'Foreign Server', sr.srvname, ':Foreign Data Wrappers/' || fdw.fdwname || '/:Foreign Servers/' || sr.srvname from pg_foreign_server sr ")
		             wxT("	inner join pg_foreign_data_wrapper fdw on sr.srvfdw = fdw.oid ")
		             wxT("	union ")
		             wxT("	select 'User Mappings', ro.rolname, ':Foreign Data Wrappers/' || fdw.fdwname || '/:Foreign Servers/' || sr.srvname || '/:User Mappings/' || ro.rolname from pg_user_mapping um ")
		             wxT("	inner join pg_roles ro on um.umuser = ro.oid ")
		             wxT("	inner join pg_foreign_server sr on um.umserver = sr.oid ")
		             wxT("	inner join pg_foreign_data_wrapper fdw on sr.srvfdw = fdw.oid ");
	}

	if(currentdb->BackendMinimumVersion(9, 1))
	{
		searchSQL += wxT("	union ")
		             wxT("	select 'Foreign Tables', c.relname, ':Schemas/' || ns.nspname || '/:Foreign Tables/' || c.relname from pg_foreign_table ft ")
		             wxT("	inner join pg_class c on ft.ftrelid = c.oid ")
		             wxT("	inner join pg_namespace ns on c.relnamespace = ns.oid ")
		             wxT("	union ")
		             wxT("	select 'Extensions', x.extname, ':Extensions/' || x.extname ")
		             wxT("	FROM pg_extension x	")
		             wxT("	JOIN pg_namespace n on x.extnamespace=n.oid ")
		             wxT("	join pg_available_extensions() e(name, default_version, comment) ON x.extname=e.name ")
		             wxT("	union ")
		             wxT("	SELECT 'Collations', c.collname, ':Schemas/' || n.nspname || '/:Collations/' || c.collname ")
		             wxT("	FROM pg_collation c ")
		             wxT("	JOIN pg_namespace n ON n.oid=c.collnamespace ");
	}

	searchSQL += wxT(") i ")
	             wxT("where lower(i.objectname) like '%") + txtPattern->GetValue().Lower() + wxT("%' ");
	if(cbType->GetValue() != _("All types"))
	{
		searchSQL += wxT("AND i.type = '") + aMap[cbType->GetValue()] + wxT("' ");
	}
	searchSQL += wxT("ORDER BY 1, 2");

	pgSet *set = currentdb->GetConnection()->ExecuteSet(searchSQL);
	int i = 0;
	if(set)
	{
		lcResults->DeleteAllItems();

		while(!set->Eof())
		{
			wxString objectType = set->GetVal(wxT("type"));
			wxString objectName = set->GetVal(wxT("objectname"));
			wxString ItemPath;


			/* Login Roles, Group Roles and Tablespaces are "outside" the database, so we have to adjust the path */
			if(objectType == wxT("Login Roles") || objectType == wxT("Group Roles") || objectType == wxT("Tablespaces"))
			{
				wxStringTokenizer tkz(databasePath, wxT("/"));
				wxString newPath;
				while(tkz.HasMoreTokens())
				{
					wxString token = tkz.GetNextToken();
					if(token == _("Databases"))
						break;
					ItemPath += token + wxT("/");
				}
				ItemPath += set->GetVal(wxT("path"));
			}
			else
			{
				ItemPath = databasePath + wxT("/") + set->GetVal(wxT("path"));
			}

			if(ItemPath.Contains(wxT("Schemas/information_schema")))
			{
				/* In information Schema only views and columns are displayed, nothing else */
				if(objectType == wxT("Views") || objectType == wxT("Columns"))
				{
					ItemPath.Replace(wxT(":Schemas/information_schema"), wxT(":Catalogs/ANSI/:Catalog Objects"));
					ItemPath.Replace(wxT(":Views/"), wxT(""));
				}
				else
				{
					set->MoveNext();
					continue;
				}
			}

			if(ItemPath.Contains(wxT("Schemas/pg_catalog")))
			{
				ItemPath.Replace(wxT(":Schemas/pg_catalog"), wxT(":Catalogs/PostgreSQL"));
			}

			wxListItem item;
			item.SetId(i);
			lcResults->InsertItem(item);

			wxString locTypeStr = wxGetTranslation(set->GetVal(wxT("type")));

			/* Check if viewing of the specified object is enabled in settings */
			if(!settings->GetDisplayOption(locTypeStr))
			{
				lcResults->SetItemTextColour(i, wxColour(128, 128, 128));
			}

			lcResults->SetItem(i, 0, locTypeStr);
			lcResults->SetItem(i, 1, objectName);
			lcResults->SetItem(i, 2, TranslatePath(ItemPath));
			set->MoveNext();
			i++;
		}
		delete set;
	}

	lcResults->SetColumnWidth(0, wxLIST_AUTOSIZE);
	lcResults->SetColumnWidth(1, wxLIST_AUTOSIZE);
	lcResults->SetColumnWidth(2, wxLIST_AUTOSIZE);
}

wxString dlgSearchObject::TranslatePath(wxString &path)
{
	/* Translate a path, but only word that start's with a colon (:) */
	wxStringTokenizer tkz(path, wxT("/"));
	wxString newPath;
	while(tkz.HasMoreTokens())
	{
		wxString token = tkz.GetNextToken();
		if(token.StartsWith(wxT(":")))
		{
			token = wxGetTranslation(token.AfterFirst(':'));
		}
		newPath = newPath + token.Trim() + wxT("/");
	}
	return newPath.BeforeLast('/');

}

void dlgSearchObject::OnCancel(wxCommandEvent &ev)
{
	if (IsModal())
		EndModal(wxID_CANCEL);
	else
		Destroy();
}

searchObjectFactory::searchObjectFactory(menuFactoryList *list, wxMenu *mnu, ctlMenuToolbar *toolbar) : contextActionFactory(list)
{
	mnu->Append(id, _("&Search objects...\tCtrl-G"), _("Search database for specific objects"));
}

wxWindow *searchObjectFactory::StartDialog(frmMain *form, pgObject *obj)
{
	dlgSearchObject *so = new dlgSearchObject(form, (pgDatabase *) obj);
	so->Show();
	return 0;
}

bool searchObjectFactory::CheckEnable(pgObject *obj)
{
	if (obj && obj->IsCreatedBy(databaseFactory))
		return ((pgDatabase *)obj)->GetConnected();

	return false;
}
