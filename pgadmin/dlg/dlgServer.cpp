//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// dlgServer.cpp - PostgreSQL Database Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"

// Must be after pgAdmin3.h or MSVC++ complains
#include <wx/colordlg.h>
#include <wx/clrpicker.h>
#include <wx/filepicker.h>

// Other app headers
#include "utils/misc.h"
#include "frm/frmMain.h"
#include "frm/frmHint.h"
#include "dlg/dlgServer.h"
#include "schema/pgDatabase.h"

// pointer to controls
#define txtHostAddr       CTRL_TEXT("txtHostAddr")
#define txtDescription    CTRL_TEXT("txtDescription")
#define txtService        CTRL_TEXT("txtService")
#define txtServiceID      CTRL_TEXT("txtServiceID")
#define cbDatabase        CTRL_COMBOBOX("cbDatabase")
#define txtPort           CTRL_TEXT("txtPort")
#define cbSSL             CTRL_COMBOBOX("cbSSL")
#define txtUsername       CTRL_TEXT("txtUsername")
#define stTryConnect      CTRL_STATIC("stTryConnect")
#define chkTryConnect     CTRL_CHECKBOX("chkTryConnect")
#define stStorePwd        CTRL_STATIC("stStorePwd")
#define chkStorePwd       CTRL_CHECKBOX("chkStorePwd")
#define txtRolename       CTRL_TEXT("txtRolename")
#define stRestore         CTRL_STATIC("stRestore")
#define chkRestore        CTRL_CHECKBOX("chkRestore")
#define stPassword        CTRL_STATIC("stPassword")
#define txtPassword       CTRL_TEXT("txtPassword")
#define txtDbRestriction  CTRL_TEXT("txtDbRestriction")
#define colourPicker      CTRL_COLOURPICKER("colourPicker")
#define cbGroup           CTRL_COMBOBOX("cbGroup")
#define pickerSSLCert     CTRL_FILEPICKER("pickerSSLCert")
#define pickerSSLKey      CTRL_FILEPICKER("pickerSSLKey")
#define pickerSSLRootCert CTRL_FILEPICKER("pickerSSLRootCert")
#define pickerSSLCrl      CTRL_FILEPICKER("pickerSSLCrl")
#define chkSSLCompression CTRL_CHECKBOX("chkSSLCompression")
#define nbNotebook        CTRL_NOTEBOOK("nbNotebook")

// SSH Tunnel Tab
#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
#define chkSSHTunnel		 CTRL_CHECKBOX("chkSSHTunnel")
#define txtTunnelHost		 CTRL_TEXT("txtTunnelHost")
#define txtTunnelUsername	 CTRL_TEXT("txtTunnelUsername")
#define radioBtnPassword	 CTRL_RADIOBUTTON("radioBtnPassword")
#define radioBtnKeyfile      CTRL_RADIOBUTTON("radioBtnKeyfile")
#define txtTunnelPassword	 CTRL_TEXT("txtTunnelPassword")
#define pickerPublicKeyFile	 CTRL_FILEPICKER("pickerPublicKeyFile")
#define pickerIdentityFile	 CTRL_FILEPICKER("pickerIdentityFile")
#define stPublicKeyFile		 CTRL_STATIC("stPublicKeyFile")
#define txtTunnelPort		 CTRL_TEXT("txtTunnelPort")
#endif

BEGIN_EVENT_TABLE(dlgServer, dlgProperty)
	EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),     dlgServer::OnPageSelect)
	EVT_TEXT(XRCID("txtHostAddr"),                     dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtDescription"),                  dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtService"),                      dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtServiceID"),                    dlgProperty::OnChange)
	EVT_TEXT(XRCID("cbDatabase"),                      dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbDatabase"),                  dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtPort")  ,                       dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtUsername"),                     dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtRolename"),                     dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtDbRestriction"),                dlgServer::OnChangeRestr)
	EVT_COMBOBOX(XRCID("cbSSL"),                       dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkStorePwd"),                 dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkRestore"),                  dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkTryConnect"),               dlgServer::OnChangeTryConnect)
	EVT_COLOURPICKER_CHANGED(XRCID("colourPicker"),    dlgServer::OnChangeColour)
	EVT_FILEPICKER_CHANGED(XRCID("pickerSSLCert"),     dlgServer::OnChangeFile)
	EVT_FILEPICKER_CHANGED(XRCID("pickerSSLKey"),      dlgServer::OnChangeFile)
	EVT_FILEPICKER_CHANGED(XRCID("pickerSSLRootCert"), dlgServer::OnChangeFile)
	EVT_FILEPICKER_CHANGED(XRCID("pickerSSLCrl"),      dlgServer::OnChangeFile)
	EVT_TEXT(XRCID("cbGroup"),                         dlgProperty::OnChange)
	EVT_COMBOBOX(XRCID("cbGroup"),                     dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkSSLCompression"),           dlgProperty::OnChange)
	EVT_BUTTON(wxID_OK,                                dlgServer::OnOK)
#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
	EVT_TEXT(XRCID("txtTunnelHost"),                   dlgProperty::OnChange)
	EVT_TEXT(XRCID("txtTunnelUsername"),               dlgProperty::OnChange)
	EVT_CHECKBOX(XRCID("chkSSHTunnel"),                dlgServer::OnCheckSSHTunnel)
	EVT_RADIOBUTTON(XRCID("radioBtnPassword"),         dlgServer::OnChangeAuthOption)
	EVT_RADIOBUTTON(XRCID("radioBtnKeyfile"),          dlgServer::OnChangeAuthOption)
	EVT_TEXT(XRCID("txtTunnelPort"),                   dlgProperty::OnChange)
#endif
END_EVENT_TABLE();


dlgProperty *pgServerFactory::CreateDialog(frmMain *frame, pgObject *node, pgObject *parent)
{
	return new dlgServer(this, frame, (pgServer *)node);
}


dlgServer::dlgServer(pgaFactory *f, frmMain *frame, pgServer *node)
	: dlgProperty(f, frame, wxT("dlgServer"))
{
	server = node;
	dbRestrictionOk = true;

	cbDatabase->Append(wxT("postgres"));
	cbDatabase->Append(wxT("edb"));
	cbDatabase->Append(wxT("template1"));
	wxString lastDB = settings->GetLastDatabase();
	if (lastDB != wxT("postgres") && lastDB != wxT("edb") && lastDB != wxT("template1"))
		cbDatabase->Append(lastDB);
	cbDatabase->SetSelection(0);

	txtPort->SetValue(NumToStr((long)settings->GetLastPort()));
	if (!cbSSL->GetValue().IsEmpty())
		cbSSL->SetSelection(settings->GetLastSSL());
	txtUsername->SetValue(settings->GetLastUsername());

	chkTryConnect->SetValue(true);
	chkStorePwd->SetValue(true);
	chkRestore->SetValue(true);
	if (node)
	{
		chkTryConnect->SetValue(false);
		chkTryConnect->Disable();
	}

	// Fill the group combobox
	cbGroup->Append(_("Servers"));
	ctlTree *browser = frame->GetBrowser();
	wxTreeItemId groupitem;
	wxTreeItemIdValue groupcookie, servercookie;
	pgServer *firstserver;
	if (browser->ItemHasChildren(browser->GetRootItem()))
	{
		groupitem = browser->GetFirstChild(browser->GetRootItem(), groupcookie);
		while (groupitem)
		{
			firstserver = (pgServer *)browser->GetObject(browser->GetFirstChild(groupitem, servercookie));
			if (firstserver && !firstserver->GetGroup().IsEmpty() && firstserver->GetGroup() != _("Servers"))
				cbGroup->Append(firstserver->GetGroup());
			groupitem = browser->GetNextChild(browser->GetRootItem(), groupcookie);
		}
	}

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
	EnableSSHTunnelControls(false);
	radioBtnPassword->SetValue(true);
	radioBtnKeyfile->SetValue(false);
	txtTunnelPassword->SetMaxLength(SSH_MAX_PASSWORD_LEN);
	txtTunnelPort->SetValue(NumToStr((long)DEFAULT_SSH_PORT));
#ifdef HAVE_OPENSSL_CRYPTO
	stPublicKeyFile->Show(false);
	pickerPublicKeyFile->Show(false);
#endif
#else
	for(size_t i = 0; i < nbNotebook->GetPageCount(); i++)
	{
		if(nbNotebook->GetPageText(i).compare(wxT("SSH Tunnel")) == 0)
		{
			nbNotebook->RemovePage(i);
		}
	}
#endif
}


dlgServer::~dlgServer()
{
	if (!server)
	{
		settings->SetLastDatabase(cbDatabase->GetValue());
		settings->SetLastPort(StrToLong(txtPort->GetValue()));
		settings->SetLastSSL(cbSSL->GetCurrentSelection());
		settings->SetLastUsername(txtUsername->GetValue());
	}
}


pgObject *dlgServer::GetObject()
{
	return server;
}


void dlgServer::OnOK(wxCommandEvent &ev)
{
#ifdef __WXGTK__
	if (!btnOK->IsEnabled())
		return;
#endif

	// Display the 'save password' hint if required
	if(chkStorePwd->GetValue())
	{
		if (frmHint::ShowHint(this, HINT_SAVING_PASSWORDS) == wxID_CANCEL)
			return;
	}

	// notice: changes active after reconnect
	EnableOK(false);

	if (server)
	{
		server->iSetName(GetName());
		server->iSetHostAddr(txtHostAddr->GetValue());
		server->iSetDescription(txtDescription->GetValue());
		server->iSetService(txtService->GetValue());
		if (txtServiceID->GetValue() != server->GetServiceID())
		{
			mainForm->StartMsg(_("Checking server status"));
			server->iSetServiceID(txtServiceID->GetValue());
			mainForm->EndMsg();
		}
		server->iSetPort(StrToLong(txtPort->GetValue()));
		server->iSetSSL(cbSSL->GetCurrentSelection());
		server->iSetDatabase(cbDatabase->GetValue());
		server->iSetUsername(txtUsername->GetValue());
		server->iSetRolename(txtRolename->GetValue());
		server->iSetStorePwd(chkStorePwd->GetValue());
		server->iSetRestore(chkRestore->GetValue());
		server->iSetDbRestriction(txtDbRestriction->GetValue().Trim());
		server->SetSSLCert(pickerSSLCert->GetPath());
		server->SetSSLKey(pickerSSLKey->GetPath());
		server->SetSSLRootCert(pickerSSLRootCert->GetPath());
		server->SetSSLCrl(pickerSSLCrl->GetPath());
		server->iSetSSLCompression(chkSSLCompression->GetValue());
#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
		server->iSetSSHTunnel(chkSSHTunnel->GetValue());
		server->iSetTunnelPort(StrToLong(txtTunnelPort->GetValue()));
		server->SetTunnelHost(txtTunnelHost->GetValue());
		server->SetTunnelUserName(txtTunnelUsername->GetValue());
		server->iSetAuthModePwd(radioBtnPassword->GetValue());
		server->SetTunnelPassword(txtTunnelPassword->GetValue());
		server->SetPublicKeyFile(pickerPublicKeyFile->GetPath());
		server->SetIdentityFile(pickerIdentityFile->GetPath());
#endif
		wxColour colour = colourPicker->GetColour();
		wxString sColour = colour.GetAsString(wxC2S_HTML_SYNTAX);
		server->iSetColour(sColour);
		if (cbGroup->GetValue().IsEmpty())
			cbGroup->SetValue(_("Servers"));
		if (server->GetGroup() != cbGroup->GetValue())
		{
			ctlTree *browser = mainForm->GetBrowser();
			wxTreeItemId oldgroupitem, groupitem, serveritem;
			wxTreeItemIdValue groupcookie;
			bool found;
			int total;
			wxString label, oldgroupname;

			// Duplicate server object
			pgServer *newserver = new pgServer(
			    server->GetName(),
			    server->GetHostAddr(),
			    server->GetDescription(),
			    server->GetService(),
			    server->GetDatabaseName(),
			    server->GetUsername(),
			    server->GetPort(),
			    server->GetStorePwd(),
			    server->GetRolename(),
			    server->GetRestore(),
			    server->GetSSL(),
			    server->GetColour(),
#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
			    server->GetGroup(),
			    server->GetSSHTunnel(),
			    server->GetTunnelHost(),
			    server->GetTunnelUserName(),
			    server->GetAuthModePwd(),
			    server->GetTunnelPassword(),
			    server->GetPublicKeyFile(),
			    server->GetIdentityFile(),
			    server->GetTunnelPort());
#else
			    server->GetGroup());
#endif
			newserver->iSetDbRestriction(server->GetDbRestriction().Trim());
			newserver->iSetServiceID(server->GetServiceID().Trim());
			newserver->iSetDiscoveryID(server->GetDiscoveryID().Trim());
			newserver->SetSSLCert(server->GetSSLCert());
			newserver->SetSSLKey(server->GetSSLKey());
			newserver->SetSSLRootCert(server->GetSSLRootCert());
			newserver->SetSSLCrl(server->GetSSLCrl());
			newserver->iSetSSLCompression(server->GetSSLCompression());

			// Drop the old item
			// (will also take care of dropping the pgServer item)
			oldgroupitem = browser->GetItemParent(item);
			oldgroupname = server->GetGroup();
			if (oldgroupname.IsEmpty())
				oldgroupname = _("Servers");
			browser->Delete(item);

			// Move the item
			found = false;
			if (browser->ItemHasChildren(browser->GetRootItem()))
			{
				groupitem = browser->GetFirstChild(browser->GetRootItem(), groupcookie);
				while (!found && groupitem)
				{
					if (browser->GetItemText(groupitem).StartsWith(cbGroup->GetValue()))
						found = true;
					else
						groupitem = browser->GetNextChild(browser->GetRootItem(), groupcookie);
				}
			}

			if (!found)
			{
				groupitem = browser->AppendItem(browser->GetRootItem(), cbGroup->GetValue(), browser->GetItemImage(browser->GetRootItem()));
				browser->SortChildren(browser->GetRootItem());
			}

			serveritem = browser->AppendItem(groupitem, newserver->GetFullName(), newserver->GetIconId(), -1, newserver);
			browser->SortChildren(groupitem);
			browser->Expand(groupitem);
			browser->SelectItem(serveritem);

			// Count the number of items in the old group item
			total = browser->GetChildrenCount(oldgroupitem, false);
			if (total == 0)
				browser->Delete(oldgroupitem);
			else
			{
				label = oldgroupname + wxT(" (") + NumToStr((long)total) + wxT(")");
				browser->SetItemText(oldgroupitem, label);
			}

			// Count the number of items in the new group item
			total = browser->GetChildrenCount(groupitem, false);
			label = cbGroup->GetValue() + wxT(" (") + NumToStr((long)total) + wxT(")");
			browser->SetItemText(groupitem, label);

			// Re-initialize old variables to have their new meanings
			server = newserver;
			item = serveritem;
		}
		server->iSetGroup(cbGroup->GetValue());

		mainForm->execSelChange(server->GetId(), true);
		mainForm->GetBrowser()->SetItemText(item, server->GetFullName());
		mainForm->SetItemBackgroundColour(item, wxColour(server->GetColour()));
		mainForm->StoreServers();
	}

	if (IsModal())
	{
		EndModal(wxID_OK);
		return;
	}
	else
		Destroy();
}


void dlgServer::OnChangeColour(wxColourPickerEvent &ev)
{
	dlgProperty::OnChange(ev);
}


void dlgServer::OnChangeFile(wxFileDirPickerEvent &ev)
{
	dlgProperty::OnChange(ev);
}


void dlgServer::OnChangeRestr(wxCommandEvent &ev)
{
	if (!connection || txtDbRestriction->GetValue().IsEmpty())
		dbRestrictionOk = true;
	else
	{
		wxString sql = wxT("EXPLAIN SELECT 1 FROM pg_database\n")
		               wxT("WHERE datname IN (") + txtDbRestriction->GetValue() + wxT(")");

		wxLogNull nix;
		wxString result = connection->ExecuteScalar(sql);

		dbRestrictionOk = !result.IsEmpty();
	}
	dlgProperty::OnChange(ev);
}


void dlgServer::OnPageSelect(wxNotebookEvent &event)
{
	// to prevent dlgProperty from catching it
}


wxString dlgServer::GetHelpPage() const
{
	return wxT("connect");
}


int dlgServer::GoNew()
{
	if (cbSSL->GetValue().IsEmpty())
		return Go(true);
	else
	{
		CheckChange();
		return ShowModal();
	}
}


int dlgServer::Go(bool modal)
{
	chkSSLCompression->Disable();

	cbSSL->Clear();
	cbSSL->Append(wxT(" "));

#ifdef PG_SSL
	cbSSL->Append(_("require"));
	cbSSL->Append(_("prefer"));

	if (pgConn::GetLibpqVersion() > 7.3)
	{
		cbSSL->Append(_("allow"));
		cbSSL->Append(_("disable"));
	}

	if (pgConn::GetLibpqVersion() >= 8.4)
	{
		cbSSL->Append(_("verify-ca"));
		cbSSL->Append(_("verify-full"));
	}

	if (pgConn::GetLibpqVersion() >= 9.2)
	{
		chkSSLCompression->Enable();
	}
#endif

	if (server)
	{
		if (cbDatabase->FindString(server->GetDatabaseName()) < 0)
			cbDatabase->Append(server->GetDatabaseName());
		txtHostAddr->SetValue(server->GetHostAddr());
		txtDescription->SetValue(server->GetDescription());
		txtService->SetValue(server->GetService());
		txtServiceID->SetValue(server->GetServiceID());
		txtPort->SetValue(NumToStr((long)server->GetPort()));
		cbSSL->SetSelection(server->GetSSL());
		cbDatabase->SetValue(server->GetDatabaseName());
		txtUsername->SetValue(server->GetUsername());
		chkStorePwd->SetValue(server->GetStorePwd());
		txtRolename->SetValue(server->GetRolename());
		chkRestore->SetValue(server->GetRestore());
		txtDbRestriction->SetValue(server->GetDbRestriction());
		colourPicker->SetColour(server->GetColour());
		cbGroup->SetValue(server->GetGroup());

		pickerSSLCert->SetPath(server->GetSSLCert());
		pickerSSLKey->SetPath(server->GetSSLKey());
		pickerSSLRootCert->SetPath(server->GetSSLRootCert());
		pickerSSLCrl->SetPath(server->GetSSLCrl());

		chkSSLCompression->SetValue(server->GetSSLCompression());

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
		chkSSHTunnel->SetValue(server->GetSSHTunnel());
		if(server->GetSSHTunnel())
		{
			chkStorePwd->SetValue(false);
			chkStorePwd->Enable(false);
		}
		txtTunnelPort->SetValue(NumToStr((long)server->GetTunnelPort()));
		txtTunnelHost->SetValue(server->GetTunnelHost());
		txtTunnelUsername->SetValue(server->GetTunnelUserName());
		if (server->GetAuthModePwd())
		{
			radioBtnPassword->SetValue(true);
			radioBtnKeyfile->SetValue(false);
		}
		else
		{
			radioBtnPassword->SetValue(false);
			radioBtnKeyfile->SetValue(true);
		}
		txtTunnelPassword->SetValue(server->GetTunnelPassword());
		pickerPublicKeyFile->SetPath(server->GetPublicKeyFile());
		pickerIdentityFile->SetPath(server->GetIdentityFile());
#endif
		stPassword->Disable();
		txtPassword->Disable();
		if (connection)
		{
			txtHostAddr->Disable();
			txtDescription->Disable();
			txtService->Disable();
			txtServiceID->Disable();
			txtName->Disable();
			cbDatabase->Disable();
			txtPort->Disable();
			cbSSL->Disable();
			txtUsername->Disable();
			chkStorePwd->Disable();
			txtRolename->Disable();
			chkRestore->Disable();
			txtDbRestriction->Disable();
			colourPicker->Disable();
			cbGroup->Disable();
			pickerSSLCert->Disable();
			pickerSSLKey->Disable();
			pickerSSLRootCert->Disable();
			pickerSSLCrl->Disable();
			chkSSLCompression->Disable();
			EnableOK(false);
#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
			chkSSHTunnel->Enable(false);
			EnableSSHTunnelControls(false);
		}
		else
		{
			if (server->GetSSHTunnel())
			{
				chkSSHTunnel->Enable(true);
				EnableSSHTunnelControls(true);
				EnableAuthenticationOptions();
			}
#endif
		}
	}
	else
	{
		SetTitle(_("Add server"));
		cbGroup->SetValue(_("Servers"));
		wxString colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString(wxC2S_HTML_SYNTAX);
		colourPicker->SetColour(colour);
	}

	// Call CheckRange to set state on OK button
	CheckChange();
	return dlgProperty::Go(modal);
}


bool dlgServer::GetTryConnect()
{
	return chkTryConnect->GetValue();
}


wxString dlgServer::GetPassword()
{
	return txtPassword->GetValue();
}


pgObject *dlgServer::CreateObject(pgCollection *collection)
{
	pgServer *obj = NULL;

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
	if (chkSSHTunnel->GetValue())
	{
		obj = new pgServer(GetName(), txtHostAddr->GetValue(), txtDescription->GetValue(),
		                   txtService->GetValue(), cbDatabase->GetValue(),
		                   txtUsername->GetValue(), StrToLong(txtPort->GetValue()),
		                   chkTryConnect->GetValue() && chkStorePwd->GetValue(),
		                   txtRolename->GetValue(), chkRestore->GetValue(), cbSSL->GetCurrentSelection(),
		                   colourPicker->GetColourString(), cbGroup->GetValue(),
		                   chkSSHTunnel->GetValue(), txtTunnelHost->GetValue(), txtTunnelUsername->GetValue(),
		                   radioBtnPassword->GetValue(),
		                   txtTunnelPassword->GetValue(), pickerPublicKeyFile->GetPath(),
		                   pickerIdentityFile->GetPath(), StrToLong(txtTunnelPort->GetValue()));
	}
	else
#endif
	{
		obj = new pgServer(GetName(), txtHostAddr->GetValue(), txtDescription->GetValue(),
		                   txtService->GetValue(), cbDatabase->GetValue(),
		                   txtUsername->GetValue(), StrToLong(txtPort->GetValue()),
		                   chkTryConnect->GetValue() && chkStorePwd->GetValue(),
		                   txtRolename->GetValue(), chkRestore->GetValue(), cbSSL->GetCurrentSelection(),
		                   colourPicker->GetColourString(), cbGroup->GetValue());
	}

	obj->iSetDbRestriction(txtDbRestriction->GetValue().Trim());
	obj->iSetServiceID(txtServiceID->GetValue());
	obj->SetSSLCert(pickerSSLCert->GetTextCtrlValue());
	obj->SetSSLKey(pickerSSLKey->GetTextCtrlValue());
	obj->SetSSLRootCert(pickerSSLRootCert->GetTextCtrlValue());
	obj->SetSSLCrl(pickerSSLCrl->GetTextCtrlValue());
	obj->iSetSSLCompression(chkSSLCompression->GetValue());

	return obj;
}


void dlgServer::OnChangeTryConnect(wxCommandEvent &ev)
{
	chkStorePwd->Enable(chkTryConnect->GetValue() && !connection);
	txtPassword->Enable(chkTryConnect->GetValue() && !connection);
	OnChange(ev);
}


void dlgServer::CheckChange()
{
	wxString name = GetName();
	bool enable = true;

	if (server)
	{
		// Get old value
		wxColour colour;
		wxString sColour = wxEmptyString;

		if (colour.Set(server->GetColour()))
			sColour = colour.GetAsString(wxC2S_HTML_SYNTAX);

		// Get new value
		wxString sColour2 = colourPicker->GetColourString();

		enable =  name != server->GetName()
		          || txtHostAddr->GetValue() != server->GetHostAddr()
		          || txtDescription->GetValue() != server->GetDescription()
		          || txtService->GetValue() != server->GetService()
		          || txtServiceID->GetValue() != server->GetServiceID()
		          || StrToLong(txtPort->GetValue()) != server->GetPort()
		          || cbDatabase->GetValue() != server->GetDatabaseName()
		          || txtUsername->GetValue() != server->GetUsername()
		          || cbSSL->GetCurrentSelection() != server->GetSSL()
		          || chkStorePwd->GetValue() != server->GetStorePwd()
		          || txtRolename->GetValue() != server->GetRolename()
		          || chkRestore->GetValue() != server->GetRestore()
		          || txtDbRestriction->GetValue() != server->GetDbRestriction()
		          || sColour != sColour2
		          || cbGroup->GetValue() != server->GetGroup()
		          || pickerSSLCert->GetPath() != server->GetSSLCert()
		          || pickerSSLKey->GetPath() != server->GetSSLKey()
		          || pickerSSLRootCert->GetPath() != server->GetSSLRootCert()
		          || pickerSSLCrl->GetPath() != server->GetSSLCrl()
		          || chkSSLCompression->GetValue() != server->GetSSLCompression();
	}

#ifdef __WXMSW__
	CheckValid(enable, !name.IsEmpty(), _("Please specify address."));
#else
	bool isPipe = (name.IsEmpty() || name.StartsWith(wxT("/")));
	cbSSL->Enable(!isPipe && !connection);
#endif
	CheckValid(enable, !txtDescription->GetValue().IsEmpty(), _("Please specify description."));
	if (txtService->GetValue().IsEmpty())
	{
		CheckValid(enable, StrToLong(txtPort->GetValue()) > 0, _("Please specify port."));
		CheckValid(enable, !txtUsername->GetValue().IsEmpty(), _("Please specify user name"));
	}
	CheckValid(enable, dbRestrictionOk, _("Restriction not valid."));

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)
	if(chkSSHTunnel->GetValue())
	{
		CheckValid(enable, !txtTunnelHost->GetValue().IsEmpty(), _("Please specify ssh tunnel host."));
		CheckValid(enable, !txtTunnelPort->GetValue().IsEmpty(), _("Please specify ssh tunnel port."));
		CheckValid(enable, !txtTunnelUsername->GetValue().IsEmpty(), _("Please specify ssh tunnel user name."));
	}
#endif

	EnableOK(enable && !connection);
}


wxString dlgServer::GetSql()
{
	return wxEmptyString;
}

#if defined(HAVE_OPENSSL_CRYPTO) || defined(HAVE_GCRYPT)

void dlgServer::OnCheckSSHTunnel(wxCommandEvent &ev)
{
	if (chkSSHTunnel->IsChecked())
	{
		if(chkStorePwd->GetValue())
		{
			wxMessageBox(_("Database passwords cannot be stored when using SSH tunnelling. The 'Store password' option has been turned off."), _("Stored Password"), wxICON_EXCLAMATION | wxOK, this);
		}
		chkStorePwd->SetValue(false);
		chkStorePwd->Enable(false);
		EnableSSHTunnelControls(true);
		EnableAuthenticationOptions();
	}
	else
	{
		chkStorePwd->Enable();
		EnableSSHTunnelControls(false);
	}

	CheckChange();
}

void dlgServer::OnChangeAuthOption(wxCommandEvent &ev)
{
	EnableAuthenticationOptions();
}

void dlgServer::EnableSSHTunnelControls(const bool &bEnable)
{
	txtTunnelHost->Enable(bEnable);
	txtTunnelUsername->Enable(bEnable);
	pickerPublicKeyFile->Enable(bEnable);
	pickerIdentityFile->Enable(bEnable);
	radioBtnPassword->Enable(bEnable);
	radioBtnKeyfile->Enable(bEnable);
	txtTunnelPassword->Enable(bEnable);
	txtTunnelPort->Enable(bEnable);
}

void dlgServer::EnableAuthenticationOptions()
{
	if (radioBtnPassword->GetValue())
	{
		pickerIdentityFile->Enable(false);
		pickerPublicKeyFile->Enable(false);
	}
	else
	{
		pickerIdentityFile->Enable(true);
		pickerPublicKeyFile->Enable(true);
	}
}
#endif
