//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgSequence.cpp - PostgreSQL Sequence Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "utils/misc.h"
#include "dlg/dlgSequence.h"

#include "schema/pgSchema.h"
#include "schema/pgSequence.h"
#include "ctl/ctlSeclabelPanel.h"


#define txtIncrement        CTRL_TEXT("txtIncrement")
#define cbOwner             CTRL_COMBOBOX2("cbOwner")
#define txtMin              CTRL_TEXT("txtMin")
#define txtMax              CTRL_TEXT("txtMax")
#define txtStart            CTRL_TEXT("txtStart")
#define txtCache            CTRL_TEXT("txtCache")
#define chkCycled           CTRL_CHECKBOX("chkCycled")
#define stStart             CTRL_STATIC("stStart")

// pointer to controls

BEGIN_EVENT_TABLE(dlgSequence, dlgSecurityProperty)
	EVT_TEXT(XRCID("txtStart"),                     dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtMin"),                       dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtMax"),                       dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtCache"),                     dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtIncrement"),                 dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkCycled"),                dlgProperty::OnChange)
END_EVENT_TABLE();


dlgProperty *pgSequenceFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgSequence(this, frame, (pgSequence *)node, (pgSchema *)parent);
}


dlgSequence::dlgSequence(pgaFactory *f, frmMain *frame, pgSequence *node, pgSchema *sch)
	: dlgSecurityProperty(f, frame, node, wxT("dlgSequence"), wxT("INSERT,SELECT,UPDATE,DELETE,RULE,REFERENCES,TRIGGER,USAGE"), "arwdRxtU")
{
	schema = sch;
	sequence = node;
	seclabelPage = new ctlSeclabelPanel(nbNotebook);
}


pgObject *dlgSequence::GetObject()
{
	return sequence;
}


int dlgSequence::Go(bool modal)
{
	if (connection->BackendMinimumVersion(9, 1))
	{
		seclabelPage->SetConnection(connection);
		seclabelPage->SetObject(sequence);
		this->Connect(EVT_SECLABELPANEL_CHANGE, wxCommandEventHandler(dlgSequence::OnChange));
	}
	else
		seclabelPage->Disable();

	if (sequence)
	{
		// edit mode
		txtIncrement->SetValue(sequence->GetIncrement().ToString());
		txtStart->SetValue(sequence->GetLastValue().ToString());
		txtMin->SetValue(sequence->GetMinValue().ToString());
		txtMax->SetValue(sequence->GetMaxValue().ToString());
		txtCache->SetValue(sequence->GetCacheValue().ToString());
		chkCycled->SetValue(sequence->GetCycled());

		stStart->SetLabel(_("Current value"));

		cbSchema->Enable(connection->BackendMinimumVersion(8, 1));

		if (!connection->BackendMinimumVersion(7, 4))
		{
			txtIncrement->Disable();
			txtMin->Disable();
			txtMax->Disable();
			txtCache->Disable();
			chkCycled->Disable();
		}
	}
	else
	{
		// create mode
		txtIncrement->SetValidator(numericValidator);
		txtMin->SetValidator(numericValidator);
		txtMax->SetValidator(numericValidator);
		txtCache->SetValidator(numericValidator);
	}

	txtStart->SetValidator(numericValidator);

	// Find, and disable the USAGE ACL option if we're on pre 8.2
	// 8.2+ only supports SELECT, UPDATE and USAGE
	if (!connection->BackendMinimumVersion(8, 2))
	{
		// Disable the checkbox
		if (!DisablePrivilege(wxT("USAGE")))
		{
			wxLogError(_("Failed to disable the USAGE privilege checkbox!"));
		}
	}
	else
	{
		if (!DisablePrivilege(wxT("INSERT")))
		{
			wxLogError(_("Failed to disable the INSERT privilege checkbox!"));
		}
		if (!DisablePrivilege(wxT("DELETE")))
		{
			wxLogError(_("Failed to disable the DELETE privilege checkbox!"));
		}
		if (!DisablePrivilege(wxT("RULE")))
		{
			wxLogError(_("Failed to disable the RULE privilege checkbox!"));
		}
		if (!DisablePrivilege(wxT("REFERENCES")))
		{
			wxLogError(_("Failed to disable the REFERENCES privilege checkbox!"));
		}
		if (!DisablePrivilege(wxT("TRIGGER")))
		{
			wxLogError(_("Failed to disable the TRIGGER privilege checkbox!"));
		}
	}

	return dlgSecurityProperty::Go(modal);
}


pgObject *dlgSequence::CreateObject(pgCollection *collection)
{
	pgObject *obj = sequenceFactory.CreateObjects(collection, 0,
	                wxT("   AND relname=") + qtDbString(GetName()) +
	                wxT("\n   AND relnamespace=") + schema->GetOidStr());

	return obj;
}


#ifdef __WXMAC__
void dlgSequence::OnChangeSize(wxSizeEvent &ev)
{
	SetPrivilegesLayout();
	if (GetAutoLayout())
	{
		Layout();
	}
}
#endif


void dlgSequence::CheckChange()
{
	bool enable = true;
	wxString name = GetName();
	bool maxOk = true;

	if (statusBar)
		statusBar->SetStatusText(wxEmptyString);

	// Check we don't overflow INT64_MAX
	if (doesOverflowBigInt(txtCache->GetValue(), !sequence))
	{
		if (statusBar)
			statusBar->SetStatusText(_("Invalid cache value"));
		maxOk = false;
	}

	if (doesOverflowBigInt(txtMax->GetValue(), !sequence))
	{
		if (statusBar)
			statusBar->SetStatusText(_("Invalid maximum value"));
		maxOk = false;
	}

	if (doesOverflowBigInt(txtMin->GetValue(), !sequence))
	{
		if (statusBar)
			statusBar->SetStatusText(_("Invalid minimum value"));
		maxOk = false;
	}

	if (doesOverflowBigInt(txtStart->GetValue(), !sequence))
	{
		if (statusBar)
			statusBar->SetStatusText(_("Invalid current value"));
		maxOk = false;
	}

	if (doesOverflowBigInt(txtIncrement->GetValue(), !sequence))
	{
		if (statusBar)
			statusBar->SetStatusText(_("Invalid increment value"));
		maxOk = false;
	}

	if (sequence)
	{
		enable = maxOk && (name != sequence->GetName()
		                   || cbSchema->GetValue() != sequence->GetSchema()->GetName()
		                   || txtComment->GetValue() != sequence->GetComment()
		                   || cbOwner->GetValue() != sequence->GetOwner()
		                   || txtStart->GetValue() != sequence->GetLastValue().ToString()
		                   || txtMin->GetValue() != sequence->GetMinValue().ToString()
		                   || txtMax->GetValue() != sequence->GetMaxValue().ToString()
		                   || txtCache->GetValue() != sequence->GetCacheValue().ToString()
		                   || txtIncrement->GetValue() != sequence->GetIncrement().ToString()
		                   || chkCycled->GetValue() != sequence->GetCycled());
		if (seclabelPage && connection->BackendMinimumVersion(9, 1))
			enable = enable || !(seclabelPage->GetSqlForSecLabels().IsEmpty());
		EnableOK(enable);
	}
	else
	{
		CheckValid(enable, !name.IsEmpty(), _("Please specify name."));
		EnableOK(enable && maxOk);
	}
}

bool dlgSequence::doesOverflowBigInt(const wxString &str, bool emptyAllowed)
{
	if (emptyAllowed && str.IsEmpty())
		return false;

	if (NumToStr(StrToLongLong(str)) != str)
		return true;

	return false;
}

wxString dlgSequence::GetSql()
{
	wxString sql, name;

	if (sequence)
	{
		// edit mode
		name = GetName();

		if (connection->BackendMinimumVersion(8, 3))
			AppendNameChange(sql, wxT("SEQUENCE ") + sequence->GetQuotedFullIdentifier());
		else
			AppendNameChange(sql, wxT("TABLE ") + sequence->GetQuotedFullIdentifier());

		if (connection->BackendMinimumVersion(8, 4))
			AppendOwnerChange(sql, wxT("SEQUENCE ") + schema->GetQuotedPrefix() + qtIdent(name));
		else
			AppendOwnerChange(sql, wxT("TABLE ") + schema->GetQuotedPrefix() + qtIdent(name));

		// This is where things get hairy. Per some thought by Horvath Gabor,
		// we need to adjust the min/max sequence values, and the current
		// value per the rules:
		//
		// 1 Any ALTER SEQUENCE MIN/MAXVALUE statements that widen the range
		// 2 SETVAL
		// 3 Any ALTER SEQUENCE MIN/MAXVALUE statements that narrow the range.
		//
		// We'll change any other options at the end.
		wxString tmp;

		// MIN/MAX changes that widen the range.
		if (connection->BackendMinimumVersion(7, 4))
		{
			tmp = wxEmptyString;
			if (txtMin->GetValue().IsEmpty())
				tmp += wxT("\n   NO MINVALUE");
			else if (StrToLongLong(txtMin->GetValue()) < sequence->GetMinValue())
				tmp += wxT("\n   MINVALUE ") + txtMin->GetValue();

			if (txtMax->GetValue().IsEmpty())
				tmp += wxT("\n   NO MAXVALUE");
			else if (StrToLongLong(txtMax->GetValue()) > sequence->GetMaxValue())
				tmp += wxT("\n   MAXVALUE ") + txtMax->GetValue();

			if (!tmp.IsEmpty())
			{
				sql += wxT("ALTER SEQUENCE ") + schema->GetQuotedPrefix() + qtIdent(name)
				       +  tmp + wxT(";\n");
			}
		}

		// The new sequence value
		if (txtStart->GetValue() != sequence->GetLastValue().ToString())
			sql += wxT("SELECT setval('") + qtIdent(schema->GetName()) + wxT(".") + qtIdent(name)
			       +  wxT("', ") + txtStart->GetValue()
			       +  wxT(", true);\n");

		// Min/Max changes that narrow the ranges, as well as other changes.
		if (connection->BackendMinimumVersion(7, 4))
		{
			tmp = wxEmptyString;
			if (txtIncrement->GetValue() != sequence->GetIncrement().ToString())
				tmp += wxT("\n   INCREMENT ") + txtIncrement->GetValue();

			if ((!txtMin->GetValue().IsEmpty()) && StrToLongLong(txtMin->GetValue()) > sequence->GetMinValue())
				tmp += wxT("\n   MINVALUE ") + txtMin->GetValue();

			if ((!txtMax->GetValue().IsEmpty()) && StrToLongLong(txtMax->GetValue()) < sequence->GetMaxValue())
				tmp += wxT("\n   MAXVALUE ") + txtMax->GetValue();

			if (txtCache->GetValue() != sequence->GetCacheValue().ToString())
				tmp += wxT("\n   CACHE ") + txtCache->GetValue();

			if (chkCycled->GetValue() != sequence->GetCycled())
			{
				if (chkCycled->GetValue())
					tmp += wxT("\n   CYCLE");
				else
					tmp += wxT("\n   NO CYCLE");
			}

			if (!tmp.IsEmpty())
			{
				sql += wxT("ALTER SEQUENCE ") + schema->GetQuotedPrefix() + qtIdent(name)
				       +  tmp + wxT(";\n");
			}

			if (connection->BackendMinimumVersion(8, 1))
				AppendSchemaChange(sql,  wxT("SEQUENCE ") + schema->GetQuotedPrefix() + qtIdent(name));
		}
	}
	else
	{
		name = qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName());

		// create mode
		sql = wxT("CREATE SEQUENCE ") + name;
		if (chkCycled->GetValue())
			sql += wxT(" CYCLE");
		AppendIfFilled(sql, wxT("\n   INCREMENT "), txtIncrement->GetValue());
		AppendIfFilled(sql, wxT("\n   START "), txtStart->GetValue());
		AppendIfFilled(sql, wxT("\n   MINVALUE "), txtMin->GetValue());
		AppendIfFilled(sql, wxT("\n   MAXVALUE "), txtMax->GetValue());
		AppendIfFilled(sql, wxT("\n   CACHE "), txtCache->GetValue());
		sql += wxT(";\n");

		if (cbOwner->GetGuessedSelection() > 0)
		{
			if (connection->BackendMinimumVersion(8, 4))
			{
				AppendOwnerChange(sql, wxT("SEQUENCE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));
			}
			else
			{
				AppendOwnerChange(sql, wxT("TABLE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));
			}
		}
	}

	if (!connection->BackendMinimumVersion(8, 2))
		sql +=  GetGrant(wxT("arwdRxt"), wxT("TABLE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));
	else
		sql +=  GetGrant(wxT("rwU"), wxT("SEQUENCE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));

	AppendComment(sql, wxT("SEQUENCE ") + qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()), sequence);

	if (seclabelPage && connection->BackendMinimumVersion(9, 1))
		sql += seclabelPage->GetSqlForSecLabels(wxT("SEQUENCE"), qtIdent(cbSchema->GetValue()) + wxT(".") + qtIdent(GetName()));

	return sql;
}

void dlgSequence::OnChange(wxCommandEvent &event)
{
	CheckChange();
}
