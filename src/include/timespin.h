///////////////////////////////////////////////////////////////////////////////
// Name:        timespin.h
// Purpose:     timeSpan SpinCtrl
// Author:      Andreas Pflug
// Modified by:
// Created:     19.10.03
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Andreas Pflug <pgadmin@pse-consulting.de>
// Licence:     Artistic Licence
///////////////////////////////////////////////////////////////////////////////


#ifndef _WX_TIMESPIN_H_
#define _WX_TIMESPIN_H_

#include "wx/datetime.h"
#include "wx/spinbutt.h"


class wxTimeSpinCtrl : public wxControl
{
public:
    wxTimeSpinCtrl() { Init(); }
    wxTimeSpinCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxWANTS_CHARS, const wxString& name=wxT("wxTimeSpinCtrl"));

    bool Create(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxWANTS_CHARS, const wxString& name=wxT("wxTimeSpinCtrl"));

    bool Destroy();
    bool Enable(bool enable);

    void SetMax(long seconds, bool useDay=false);
    bool SetValue(const wxTimeSpan& span);
    bool SetTime(const wxDateTime& time);
    wxTimeSpan GetValue();

private:
    void Init();

    void OnSpinUp(wxSpinEvent &ev);
    void OnSpinDown(wxSpinEvent &ev);
    void OnText(wxNotifyEvent &ev);
    void OnSetFocus(wxFocusEvent &ev);
    void OnKillFocus(wxFocusEvent &ev);
    void OnEditKey(wxKeyEvent &ev);
    void OnNavigate(wxNavigationKeyEvent &ev);

    long GetTextTime();
    int  GetTimePart();
    void DoSpin(int diff);
    void Highlight(int tp);

    wxTextCtrl *m_txt;
    wxSpinButton *m_spn;
    wxString m_format;
    long spinValue, maxSpinValue;
    bool canWrap, hasDay;

    DECLARE_DYNAMIC_CLASS(wxTimeSpinCtrl)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxTimeSpinCtrl)
};


#if 0
class wxTimeSpinCtrl : public wxSpinCtrl //wxControl
{
public:
    wxTimeSpinCtrl() { }
    wxTimeSpinCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxWANTS_CHARS, const wxString& name=wxT("wxTimeSpinCtrl"));

    bool Destroy();


    bool Create(wxWindow *parent,
                            wxWindowID id,
                            const wxDateTime& date,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name);

    bool SetValue(const wxTimeSpan& span);
    wxTimeSpan GetValue();

    virtual bool Enable(bool enable = TRUE);
    virtual bool Show(bool show = TRUE);
    virtual void DoMoveWindow(int x, int y, int width, int height);

private:
    wxTextCtrl *m_txt;
    wxSpinCtrl *m_spn;
    wxButton *m_btn;
    wxString m_format;

    bool m_dropped, m_processing;

    void Init();
    void DropDown(bool down=true);

    void OnEditKey(wxKeyEvent & event);
    void OnCalKey(wxKeyEvent & event);
    void OnClick(wxMouseEvent &ev);
    void OnSelChange(wxCalendarEvent &ev);
    void OnActivate(wxActivateEvent &ev);
    void OnSetFocus(wxFocusEvent &ev);
    void OnKillFocus(wxFocusEvent &ev);

    DECLARE_DYNAMIC_CLASS(wxTimeSpinCtrl)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxTimeSpinCtrl)
};
#endif


#endif // _WX_TIMESPIN_H_

