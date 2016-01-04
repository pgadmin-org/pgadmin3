//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// utffile.h - file io with BOM interpretation
//
//////////////////////////////////////////////////////////////////////////


#ifndef _WX_UTFFILEH__
#define _WX_UTFFILEH__

#include  "wx/wx.h"
#include  "wx/font.h"
#include  "wx/file.h"

class wxUtfFile : public wxFile
{
public:
	wxUtfFile();
	wxUtfFile(const wxChar *szFileName, OpenMode mode = read, wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
	wxUtfFile(int fd, wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

	bool Create(const wxChar *szFileName, bool bOverwrite = false, int access = wxS_DEFAULT, wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
	bool Open(const wxChar *szFileName, OpenMode mode = read, int access = wxS_DEFAULT, wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
	void Attach(int fd, wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

	wxFontEncoding GetEncoding();

	off_t Seek(off_t ofs, wxSeekMode mode = wxFromStart);
	off_t SeekEnd(off_t ofs = 0)
	{
		return Seek(ofs, wxFromEnd);
	}
	off_t Tell() const
	{
		return wxFile::Tell() - m_bomOffset;
	}
	off_t Length() const
	{
		return wxFile::Length() - m_bomOffset;
	}

	off_t Read(wxString &str, off_t nCount = (off_t) - 1);
	bool Write(const wxString &str);

protected:

	void WriteBOM();
	void DetermineConversion(wxFontEncoding encoding);
	bool EvalBOM(wxFontEncoding encoding);

	wxMBConv *m_conversion;
	wxFontEncoding m_encoding;
	off_t m_bomOffset;
	wxString m_strFileName;
};

#endif // _WX_UTFFILEH__
