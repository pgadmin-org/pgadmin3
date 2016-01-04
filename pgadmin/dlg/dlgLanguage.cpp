//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgLanguage.cpp - Language properties dialog
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "utils/pgDefs.h"

#include "dlg/dlgLanguage.h"
#include "schema/pgLanguage.h"
#include "ctl/ctlSeclabelPanel.h"

// pointer to controls
#define cbName          CTRL_COMBOBOX("cbName")
#define chkTrusted      CTRL_CHECKBOX("chkTrusted")
#define cbHandler       CTRL_COMBOBOX("cbHandler")
#define cbInline        CTRL_COMBOBOX("cbInline")
#define cbValidator     CTRL_COMBOBOX("cbValidator")

dlgProperty *pgLanguageFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgLanguage(this, frame, (pgLanguage *)node, parent);
}

BEGIN_EVENT_TABLE(dlgLanguage, dlgSecurityProperty)
	EVT_TEXT(XRCID("cbName"),                       dlgLanguage::OnChangeName)
	EVT_COMBOBOX(XRCID("cbName"),                   dlgLanguage::OnChangeName)
	EVT_TEXT(XRCID("cbHandler"),                    dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbHandler"),                dlgProperty::OnChange)
END_EVENT_TABLE();

dlgLanguage::dlgLanguage(pgaFactory *f, frmMain *frame, pgLanguage *node, pgObject *parent)
	: dlgSecurityProperty(f, frame, node, wxT("dlgLanguage"), wxT("USAGE"), "U")
{
	language = node;
	seclabelPage = new ctlSeclabelPanel(nbNotebook);

	if (!node)
	{
		int icon = PGICON_PUBLIC;
		wxString name = wxT("public");
		wxString value = wxT("U");
		securityPage->lbPrivileges->AppendItem(icon, name, value);
		AppendCurrentAcl(name, value);
	}
}

pgObject *dlgLanguage::GetObject()
{
	return language;
}

int dlgLanguage::Go(bool modal)
{
	if (!connection->BackendMinimumVersion(8, 3))
		cbOwner->Disable();

	if (!connection->BackendMinimumVersion(7, 5))
		txtComment->Disable();

	if (!connection->BackendMinimumVersion(9, 0))
		cbInline->Disable();

	if (connection->BackendMinimumVersion(9, 1))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(language);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgLanguage::OnChange));
	}
	else
		seclabelPage->Disable();

	if (language)
	{
		// edit mode
		chkTrusted->SetValue(language->GetTrusted());

		cbHandler->Append(language->GetHandlerProc());
		cbHandler->SetSelection(0);

		if (connection->BackendMinimumVersion(9, 0))
		{
			cbInline->Append(language->GetInlineProc());
			cbInline->SetSelection(0);
		}

		wxString val = language->GetValidatorProc();
		if (!val.IsEmpty())
		{
			cbValidator->Append(val);
			cbValidator->SetSelection(0);
		}

		cbName->SetValue(language->GetName());
		if (!connection->BackendMinimumVersion(7, 4))
			cbName->Disable();

		chkTrusted->Disable();
		cbHandler->Disable();
		cbInline->Disable();
		cbValidator->Disable();
	}
	else
	{
		// create mode
		if (connection->BackendMinimumVersion(8, 1))
		{
			pgSetIterator languages(connection,
			                        wxT("SELECT tmplname FROM pg_pltemplate\n")
			                        wxT("  LEFT JOIN pg_language ON tmplname=lanname\n")
			                        wxT(" WHERE lanname IS NULL\n")
			                        wxT(" ORDER BY tmplname"));

			while (languages.RowsLeft())
				cbName->Append(languages.GetVal(wxT("tmplname")));
		}
		else
		{
			// to clear drop down list
			cbName->Append(wxT(" "));
			cbName->Delete(0);
		}

		cbValidator->Append(wxT(""));
		pgSet *set = connection->ExecuteSet(
		                 wxT("SELECT nspname, proname, prorettype, proargtypes[0] AS argtype\n")
		                 wxT("  FROM pg_proc p\n")
		                 wxT("  JOIN pg_namespace nsp ON nsp.oid=pronamespace\n")
		                 wxT(" WHERE prorettype=") + NumToStr(PGOID_TYPE_LANGUAGE_HANDLER) + wxT("\n")
		                 wxT("    OR (prorettype=") + NumToStr(PGOID_TYPE_VOID) +
		                 wxT("        AND proargtypes[0]=") + NumToStr(PGOID_TYPE_LANGUAGE_HANDLER) + wxT(")")
		                 wxT("    OR (prorettype=") + NumToStr(PGOID_TYPE_VOID) +
		                 wxT("        AND proargtypes[0]=") + NumToStr(PGOID_TYPE_INTERNAL) + wxT(")"));
		if (set)
		{
			while (!set->Eof())
			{
				wxString procname = database->GetSchemaPrefix(set->GetVal(wxT("nspname"))) + set->GetVal(wxT("proname"));

				if (set->GetOid(wxT("prorettype")) == PGOID_TYPE_LANGUAGE_HANDLER)
					cbHandler->Append(procname);
				else
				{
					if (set->GetOid(wxT("argtype")) == PGOID_TYPE_LANGUAGE_HANDLER)
						cbValidator->Append(procname);
					else
						cbInline->Append(procname);
				}
				set->MoveNext();
			}
			delete set;
		}
		cbHandler->SetSelection(0);
		cbInline->SetSelection(0);
		cbValidator->SetSelection(0);
	}

	return dlgSecurityProperty::Go(modal);
}

pgObject *dlgLanguage::CreateObject(pgCollection *collection)
{
	wxString name = cbName->wxComboBox::GetValue();

	pgObject *obj = languageFactory.CreateObjects(collection, 0, wxT("\n   AND lanname ILIKE ") + qtDbString(name));
	return obj;
}

void dlgLanguage::OnChangeName(wxCommandEvent &ev)
{
	if (connection->BackendMinimumVersion(8, 1) && !language)
	{
		bool useTemplate = (cbName->FindString(cbName->wxComboBox::GetValue()) >= 0);
		chkTrusted->Enable(!useTemplate);
		cbHandler->Enable(!useTemplate);
		cbInline->Enable(!useTemplate && connection->BackendMinimumVersion(9, 0));
		cbValidator->Enable(!useTemplate);
	}
	OnChange(ev);
}

void dlgLanguage::CheckChange()
{
	bool enable = true;
	wxString name = cbName->GetValue();
	if (language)
	{
		enable = name != language->GetName()
		         || txtComment->GetValue() != language->GetComment()
		         || (connection->BackendMinimumVersion(8, 3) && cbOwner->GetValue() != language->GetOwner());
		if (seclabelPage && connection->BackendMinimumVersion(9, 1))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());
	}
	else
	{
		bool useTemplate = (cbName->FindString(name) >= 0);

		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		CheckValid(enable, useTemplate || !cbHandler->GetValue().IsEmpty(), _("Please specify language handler."));
	}
	EnableOK(enable);
}

wxString dlgLanguage::GetSql()
{
	wxString sql, name;
	name = cbName->GetValue();

	if (language)
	{
		// edit mode
		if (name != language->GetName())
			sql += wxT("ALTER LANGUAGE ") + qtIdent(language->GetName())
			       +  wxT("\n  RENAME TO ") + qtIdent(name) + wxT(";\n");
		if (connection->BackendMinimumVersion(8, 3))
			AppendOwnerChange(sql, wxT("LANGUAGE ") + qtIdent(name));
	}
	else
	{
		// create mode
		if (connection->BackendMinimumVersion(8, 1) && cbName->FindString(name) >= 0)
		{
			sql = wxT("CREATE LANGUAGE ") + qtIdent(name) + wxT(";\n");
		}
		else
		{
			sql = wxT("CREATE ");
			if (chkTrusted->GetValue())
				sql += wxT("TRUSTED ");
			sql += wxT("LANGUAGE ") + qtIdent(name) + wxT("\n   HANDLER ") + qtIdent(cbHandler->GetValue());
			if (connection->BackendMinimumVersion(9, 0))
				AppendIfFilled(sql, wxT("\n   INLINE "), qtIdent(cbInline->GetValue()));
			AppendIfFilled(sql, wxT("\n   VALIDATOR "), qtIdent(cbValidator->GetValue()));
			sql += wxT(";\n");
		}
		if (connection->BackendMinimumVersion(8, 3))
			AppendOwnerNew(sql, wxT("LANGUAGE ") + qtIdent(name));
	}

	sql += GetGrant(wxT("U"), wxT("LANGUAGE ") + qtIdent(name));
	AppendComment(sql, wxT("LANGUAGE ") + qtIdent(name), 0, language);

	if (seclabelPage && connection->BackendMinimumVersion(9, 1))
		sql += seclabelPage->GetSqlForSecLabels(wxT("LANGUAGE"), qtIdent(name));

	return sql;
}

void dlgLanguage::OnChange(wxCommandEvent &event)
{
	CheckChange();
}
