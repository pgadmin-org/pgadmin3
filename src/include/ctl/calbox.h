//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2006, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// calbox.h - Date-picker control box
//
//////////////////////////////////////////////////////////////////////////

#ifndef _WX_CALBOX_H_
#define _WX_CALBOX_H_

#include "wx/calctrl.h"

class wxCalendarBox : public wxControl
{
public:
    wxCalendarBox() { Init(); }
    wxCalendarBox(wxWindow *parent,
                   wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCAL_SHOW_HOLIDAYS | wxWANTS_CHARS, const wxString& name=wxCalendarNameStr);

    bool Destroy();

    bool Create(wxWindow *parent,
                            wxWindowID id,
                            const wxDateTime& date,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name);

    bool SetValue(const wxDateTime& date);
    wxDateTime GetValue();

    bool SetLowerDateLimit(const wxDateTime& date = wxDefaultDateTime) { return m_cal->SetLowerDateLimit(date); }
    const wxDateTime& GetLowerDateLimit() const { return m_cal->GetLowerDateLimit(); }
    bool SetUpperDateLimit(const wxDateTime& date = wxDefaultDateTime) { return m_cal->SetUpperDateLimit(date); }
    const wxDateTime& GetUpperDateLimit() const { return m_cal->GetUpperDateLimit(); }

    bool SetDateRange(const wxDateTime& lowerdate = wxDefaultDateTime, const wxDateTime& upperdate = wxDefaultDateTime)
    { return m_cal->SetDateRange(lowerdate, upperdate); }

    wxCalendarDateAttr *GetAttr(size_t day) const { return m_cal->GetAttr(day); }
    void SetAttr(size_t day, wxCalendarDateAttr *attr) { m_cal->SetAttr(day, attr); }
    void SetHoliday(size_t day) { m_cal->SetHoliday(day); }
    void ResetAttr(size_t day) { m_cal->ResetAttr(day); }
    bool SetFormat(const wxChar *fmt);

    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);
    virtual void DoMoveWindow(int x, int y, int width, int height);

private:
    wxDialog *m_dlg;
    wxTextCtrl *m_txt;
    wxCalendarCtrl *m_cal;
    wxButton *m_btn;
    wxString m_format;

    bool m_dropped, m_ignoreDrop;

    void Init();
    void DropDown(bool down=true);

    wxSize DoGetBestSize() const;
    void OnSize(wxSizeEvent& event);

    void OnText(wxCommandEvent &ev);
    void OnEditKey(wxKeyEvent & event);
    void OnCalKey(wxKeyEvent & event);
    void OnClick(wxCommandEvent &ev);
    void OnSelChange(wxCalendarEvent &ev);
    void OnSetFocus(wxFocusEvent &ev);
    void OnKillFocus(wxFocusEvent &ev);
    void OnChildSetFocus(wxChildFocusEvent &ev);

    DECLARE_DYNAMIC_CLASS(wxCalendarBox)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxCalendarBox)
};

#endif // _WX_CALBOX_H_

