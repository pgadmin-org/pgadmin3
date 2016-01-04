//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// explainCanvas.h - Explain Canvas
//
//////////////////////////////////////////////////////////////////////////

#ifndef EXPLAINCANVAS_H
#define EXPLAINCANVAS_H

#if wxUSE_POPUPWIN
#include "wx/popupwin.h"

#define pgTipWindowBase wxPopupTransientWindow
#else
#include "wx/frame.h"

#define pgTipWindowBase wxFrame
#endif

#include <ogl/ogl.h>


#if wxUSE_DEPRECATED
#error wxUSE_DEPRECATED should be 0!
#endif


class ExplainShape;
class ExplainPopup;
class ExplainText;

class ExplainCanvas : public wxShapeCanvas
{
public:
	ExplainCanvas(wxWindow *parent);
	~ExplainCanvas();

	void ShowPopup(ExplainShape *s);
	void SetExplainString(const wxString &str);
	void Clear();
	void SaveAsImage(const wxString &fileName, wxBitmapType imageType);

private:
	void OnMouseMotion(wxMouseEvent &ev);

	ExplainShape *rootShape;
	ExplainPopup *popup;

	DECLARE_EVENT_TABLE()
};


class ExplainShape : public wxBitmapShape
{
public:
	ExplainShape(const wxImage &bmp, const wxString &description, long tokenNo = -1, long detailNo = -1);
	static ExplainShape *Create(long level, ExplainShape *last, const wxString &str);

	void SetCondition(const wxString &str)
	{
		if (condition.Length() == 0) condition = str;
		else condition += wxT(" ") + str;
	}
	long GetLevel()
	{
		return level;
	}
	wxRealPoint GetStartPoint();
	wxRealPoint GetEndPoint(int kidNo);
	int GetKidno()
	{
		return kidNo;
	}

	ExplainShape *GetUpper()
	{
		return upperShape;
	}
	double GetAverageCost()
	{
		return (costHigh - costLow) / 2 + costLow;
	}

protected:
	void OnDraw(wxDC &dc);
	void OnLeftClick(double x, double y, int keys = 0, int attachment = 0);

	ExplainShape *upperShape;

	void SetLabel(const wxString &str, int tokenNo = -1, int detailNo = -1);

	long level;
	wxString description, detail, condition, label;
	wxString cost, actual;
	double costLow, costHigh;
	long rows, width;
	int kidCount, kidNo;
	int totalShapes; // horizontal space usage by shape and its kids
	int usedShapes;
	bool m_rootShape;

	friend class ExplainCanvas;
	friend class ExplainText;
};


class ExplainLine : public wxLineShape
{
public:
	ExplainLine(ExplainShape *from, ExplainShape *to, double weight = 0);

private:
	int width;
	void OnDraw(wxDC &dc);
};


class ExplainPopup : public pgTipWindowBase
{
public:
	ExplainPopup(ExplainCanvas *parent, ExplainShape *shape, ExplainPopup **popup = NULL);
	void Close();
	~ExplainPopup();

protected:
	// event handlers
	void OnMouseClick(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &ev);

#if !wxUSE_POPUPWIN
	void OnActivate(wxActivateEvent &event);
	void OnKillFocus(wxFocusEvent &event);

#else // wxUSE_POPUPWIN
	virtual void OnDismiss();
	void OnMouseLost(wxMouseCaptureLostEvent &ev);
#endif // wxUSE_POPUPWIN/!wxUSE_POPUPWIN

	ExplainPopup **m_ptr;
	wxRect         m_rectBound;

	ExplainText   *m_explainText;

#if !wxUSE_POPUPWIN
	long           m_creationTime;
#endif // !wxUSE_POPUPWIN

	friend class ExplainText;

	DECLARE_EVENT_TABLE()
};

#endif

