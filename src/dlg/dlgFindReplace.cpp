//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgFindReplace.cpp - Search and replace
//
//////////////////////////////////////////////////////////////////////////



// App headers
#include "pgAdmin3.h"

#include "dlgFindReplace.h"
#include "ctl/ctlSQLBox.h"

BEGIN_EVENT_TABLE(dlgFindReplace, pgDialog)
    EVT_BUTTON (wxID_FIND,                          dlgFindReplace::OnFind)
    EVT_BUTTON (wxID_REPLACE,                       dlgFindReplace::OnReplace)
    EVT_BUTTON (XRCID("wxID_REPLACEALL"),           dlgFindReplace::OnReplaceAll)
    EVT_BUTTON (wxID_CANCEL,                        dlgFindReplace::OnCancel)
    EVT_RADIOBUTTON(XRCID("rdOriginCursor"),        dlgFindReplace::OnChange)
    EVT_RADIOBUTTON(XRCID("rdOriginTop"),           dlgFindReplace::OnChange)
    EVT_CHECKBOX(XRCID("chkOptionsUseRegexps"),     dlgFindReplace::OnChange)
    EVT_CLOSE(                                      dlgFindReplace::OnClose)
END_EVENT_TABLE()


#define btnFind			     CTRL_BUTTON("wxID_FIND")
#define btnReplace		     CTRL_BUTTON("wxID_REPLACE")
#define btnReplaceAll        CTRL_BUTTON("wxID_REPLACEALL")
#define txtFind		         CTRL_TEXT("txtFind")
#define txtReplace		     CTRL_TEXT("txtReplace")
#define rdOriginTop          CTRL_RADIOBUTTON("rdOriginTop")
#define rdOriginCursor       CTRL_RADIOBUTTON("rdOriginCursor")
#define rdDirectionForward   CTRL_RADIOBUTTON("rdDirectionForward")
#define rdDirectionBackward  CTRL_RADIOBUTTON("rdDirectionBackward")
#define chkOptionsWholeWord  CTRL_CHECKBOX("chkOptionsWholeWord")
#define chkOptionsMatchCase  CTRL_CHECKBOX("chkOptionsMatchCase")
#define chkOptionsUseRegexps CTRL_CHECKBOX("chkOptionsUseRegexps")

dlgFindReplace::dlgFindReplace(ctlSQLBox *parent) :
pgDialog()
{
    wxLogInfo(wxT("Creating a search & replace dialogue"));

    sqlbox = parent;

    wxWindowBase::SetFont(settings->GetSystemFont());
    LoadResource(parent, wxT("dlgFindReplace"));
    RestorePosition();

    // Icon
    appearanceFactory->SetIcons(this);

    // Accelerator table
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_NORMAL, WXK_F3, wxID_FIND);
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);


    // Load up the defaults
    wxString val;
    bool bVal;

    // Find/Replace values
    settings->Read(wxT("FindReplace/Find"), &val, wxT(""));
    txtFind->SetValue(val);

    settings->Read(wxT("FindReplace/Replace"), &val, wxT(""));
    txtReplace->SetValue(val);

    // Origin
    settings->Read(wxT("FindReplace/Origin"), &val, wxT("c"));
    if (val == wxT("t"))
    {
        rdOriginCursor->SetValue(false);
        rdOriginTop->SetValue(true);
    }
    else
    {
        rdOriginCursor->SetValue(true);
        rdOriginTop->SetValue(false);
    }

    // Origin
    settings->Read(wxT("FindReplace/Direction"), &val, wxT("f"));
    if (val == wxT("b"))
    {
        rdDirectionForward->SetValue(false);
        rdDirectionBackward->SetValue(true);
    }
    else
    {
        rdDirectionForward->SetValue(true);
        rdDirectionBackward->SetValue(false);
    }

    // WholeWord
    settings->Read(wxT("FindReplace/WholeWord"), &bVal, false);
    chkOptionsWholeWord->SetValue(bVal);

    // MatchCase
    settings->Read(wxT("FindReplace/MatchCase"), &bVal, false);
    chkOptionsMatchCase->SetValue(bVal);

    // UseRegexps
    settings->Read(wxT("FindReplace/UseRegexps"), &bVal, false);
    chkOptionsUseRegexps->SetValue(bVal);

    wxCommandEvent ev;
    OnChange(ev);
}

dlgFindReplace::~dlgFindReplace()
{
    wxLogInfo(wxT("Destroying a search & replace dialogue"));
    SavePosition();
}

void dlgFindReplace::FocusSearch()
{
    txtFind->SetFocus();
    txtFind->SetSelection(-1, -1);
}

void dlgFindReplace::OnClose(wxCloseEvent& ev)
{
    // Save settings
    settings->Write(wxT("FindReplace/Find"), txtFind->GetValue());
    settings->Write(wxT("FindReplace/Replace"), txtReplace->GetValue());

    if (rdOriginTop->GetValue())
        settings->Write(wxT("FindReplace/Origin"), wxT("t"));
    else
        settings->Write(wxT("FindReplace/Origin"), wxT("c"));

    if (rdDirectionBackward->GetValue())
        settings->Write(wxT("FindReplace/Direction"), wxT("b"));
    else
        settings->Write(wxT("FindReplace/Direction"), wxT("f"));

    settings->Write(wxT("FindReplace/WholeWord"), chkOptionsWholeWord->GetValue());
    settings->Write(wxT("FindReplace/MatchCase"), chkOptionsMatchCase->GetValue());
    settings->Write(wxT("FindReplace/UseRegexps"), chkOptionsUseRegexps->GetValue());

    this->Hide();
    if (ev.CanVeto())
        ev.Veto();
}

void dlgFindReplace::OnCancel(wxCommandEvent& ev)
{
    this->Hide();
}

void dlgFindReplace::OnChange(wxCommandEvent& ev)
{
    if (chkOptionsUseRegexps->GetValue() == true || rdOriginTop->GetValue() == true)
    {
        rdDirectionForward->SetValue(true);
        rdDirectionBackward->SetValue(false);
        rdDirectionBackward->Enable(false);
    }
    else
    {
        rdDirectionBackward->Enable(true);
    }
    
    if (chkOptionsUseRegexps->GetValue() == true)
    {
        chkOptionsWholeWord->Enable(false);
        chkOptionsMatchCase->Enable(false);
    }
    else
    {
        chkOptionsWholeWord->Enable(true);
        chkOptionsMatchCase->Enable(true);
    }
}

void dlgFindReplace::OnFind(wxCommandEvent& ev)
{
    if (txtFind->GetValue().IsEmpty())
        return;

    bool wholeWord = false, 
         matchCase = false, 
         useRegexps = false, 
         startAtTop = false,
         reverse = false;

    if (rdOriginTop->GetValue() == true)
        startAtTop = true;

    if (rdDirectionBackward->GetValue() == true)
        reverse = true;

    if (chkOptionsWholeWord->GetValue() == true)
        wholeWord = true;

    if (chkOptionsMatchCase->GetValue() == true)
        matchCase = true;

    if (chkOptionsUseRegexps->GetValue() == true)
        useRegexps = true;

    if (sqlbox->Find(txtFind->GetValue(), wholeWord, matchCase, useRegexps, startAtTop, reverse))
    {
        if (startAtTop)
        {
            rdOriginTop->SetValue(false);
            rdOriginCursor->SetValue(true);
            wxCommandEvent nullEvent;
            OnChange(nullEvent);
        }
    }
}

void dlgFindReplace::OnReplace(wxCommandEvent& ev)
{
    if (txtFind->GetValue().IsEmpty())
        return;

    bool wholeWord = false, 
         matchCase = false, 
         useRegexps = false, 
         startAtTop = false,
         reverse = false;

    if (rdOriginTop->GetValue() == true)
        startAtTop = true;

    if (rdDirectionBackward->GetValue() == true)
        reverse = true;

    if (chkOptionsWholeWord->GetValue() == true)
        wholeWord = true;

    if (chkOptionsMatchCase->GetValue() == true)
        matchCase = true;

    if (chkOptionsUseRegexps->GetValue() == true)
        useRegexps = true;

    if (sqlbox->Replace(txtFind->GetValue(), txtReplace->GetValue(), wholeWord, matchCase, useRegexps, startAtTop, reverse))
    {
        if (startAtTop)
        {
            rdOriginTop->SetValue(false);
            rdOriginCursor->SetValue(true);
            wxCommandEvent nullEvent;
            OnChange(nullEvent);
        }
    }
}

void dlgFindReplace::OnReplaceAll(wxCommandEvent& ev)
{
    if (txtFind->GetValue().IsEmpty())
        return;

    bool wholeWord = false, 
         matchCase = false, 
         useRegexps = false;

    if (chkOptionsWholeWord->GetValue() == true)
        wholeWord = true;

    if (chkOptionsMatchCase->GetValue() == true)
        matchCase = true;

    if (chkOptionsUseRegexps->GetValue() == true)
        useRegexps = true;

    sqlbox->ReplaceAll(txtFind->GetValue(), txtReplace->GetValue(), wholeWord, matchCase, useRegexps);
}