//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// utffile.cpp - file io with BOM interpretation
//
//////////////////////////////////////////////////////////////////////////

#include "pgAdmin3.h"
#include "utils/utffile.h"
#include "utils/sysLogger.h"

wxMBConvUTF16BE wxConvUTF16BE;
wxMBConvUTF16LE wxConvUTF16LE;
wxMBConvUTF32BE wxConvUTF32BE;
wxMBConvUTF32LE wxConvUTF32LE;

// these are the magic characters identifying an Unicode file
#define BOM_UTF8    "\357\273\277"
#define BOM_UTF16LE "\377\376"
#define BOM_UTF16BE "\376\377"
#define BOM_UTF32LE  "\377\376\000\000"
#define BOM_UTF32BE  "\000\000\376\377"

wxUtfFile::wxUtfFile() : wxFile()
{
}


wxUtfFile::wxUtfFile(const wxChar *szFileName, OpenMode mode, wxFontEncoding encoding) : wxFile()
{
	m_strFileName = szFileName;
	Open(szFileName, mode, wxS_DEFAULT, encoding);
}


wxUtfFile::wxUtfFile(int fd, wxFontEncoding encoding) : wxFile(fd)
{
	EvalBOM(encoding);
}


off_t wxUtfFile::Read(wxString &str, off_t nCount)
{
	if (nCount == (off_t) - 1)
		nCount = Length() - Tell();
	if (!nCount)
		return 0;

	char *buffer = new char[nCount + 4];
	// on some systems, len returned from wxFile::read might not reflect the number of bytes written
	// to the buffer, but the bytes read from file. In case of CR/LF translation, this is not the same.
	memset(buffer, 0, nCount + 4);
	off_t len = wxFile::Read(buffer, nCount);

	if (len >= 0)
	{
		memset(buffer + len, 0, 4);

		if (m_conversion)
		{
			int decr;
			size_t nLen = 0;


			// We are trying 4 times to convert, in case the last utf char
			// was truncated.
			for (decr = 0 ; len > 0 && decr < 4 ; decr++)
			{
				nLen = m_conversion->MB2WC(NULL, buffer, 0);
				if ( nLen != (size_t) - 1 )
					break;
				len--;
				buffer[len] = 0;
			}

			if (nLen == (size_t) - 1)
			{
				if (!m_strFileName.IsEmpty())
				{
					wxLogWarning(_("The file \"%s\" could not be opened because it contains characters that could not be interpreted."), m_strFileName.c_str());
				}
				Seek(decr - nLen, wxFromCurrent);
				return (size_t) - 1;
			}
			if (decr)
				Seek(-decr, wxFromCurrent);

			m_conversion->MB2WC((wchar_t *)(wxChar *)wxStringBuffer(str, nLen + 1), (const char *)buffer, (size_t)(nLen + 1));
		}
		else
			str = (wxChar *)buffer;
	}

	delete[] buffer;
	return len;
}


bool wxUtfFile::Write(const wxString &str)
{
	size_t len = str.Length();
	if (!len)
		return true;

	if (m_conversion)
	{
		wxWX2MBbuf buf = str.mb_str(*m_conversion);
		if (!buf)
			return false;

		return wxFile::Write(str, *m_conversion);
	}
	else
		return wxFile::Write(str.c_str(), len * sizeof(wxChar)) == len * sizeof(wxChar);
}



bool wxUtfFile::Create(const wxChar *szFileName, bool bOverwrite, int access, wxFontEncoding encoding)
{
	if (!wxFile::Create(szFileName, bOverwrite, access))
		return false;

	DetermineConversion(encoding);
	WriteBOM();

	return true;
}


bool wxUtfFile::Open(const wxChar *szFileName, OpenMode mode, int access, wxFontEncoding encoding)
{
	if (!wxFile::Open(szFileName, mode, access))
		return false;


	m_bomOffset = 0;
	DetermineConversion(wxFONTENCODING_SYSTEM);

	if (mode != write && EvalBOM(encoding))
	{
		// File freshly created, need BOM
		if (mode != read)
			WriteBOM();
	}
	else if (mode != read && encoding != wxFONTENCODING_DEFAULT)
	{
		// force BOM to a specific value
		switch (encoding)
		{
			case wxFONTENCODING_UTF8:
			case wxFONTENCODING_UTF16BE:
			case wxFONTENCODING_UTF16LE:
			case wxFONTENCODING_UTF32BE:
			case wxFONTENCODING_UTF32LE:
				break;
			default:
				encoding = wxFONTENCODING_SYSTEM;
				break;
		}
		if (encoding != m_encoding)
		{
			DetermineConversion(encoding);
			WriteBOM();
		}
	}
	return true;
}


void wxUtfFile::Attach(int fd, wxFontEncoding encoding)
{
	wxFile::Attach(fd);
	EvalBOM(encoding);
}


off_t wxUtfFile::Seek(off_t ofs, wxSeekMode mode)
{
	off_t pos;
	if (mode == wxFromStart)
		pos = wxFile::Seek(ofs + m_bomOffset, wxFromStart) - m_bomOffset;
	else
		pos = wxFile::Seek(ofs, mode) - m_bomOffset;

	if (pos != wxInvalidOffset)
		pos -= m_bomOffset;

	return pos;
}



wxFontEncoding wxUtfFile::GetEncoding()
{
	if (IsOpened())
		return m_encoding;
	else
		return wxFONTENCODING_DEFAULT;
}


void wxUtfFile::WriteBOM()
{
	if (!settings->GetWriteBOM())
		return;

	wxFile::Seek(0);
	switch (m_encoding)
	{
		case wxFONTENCODING_UTF8:
			wxFile::Write(BOM_UTF8, 3);
			m_bomOffset = 3;
			break;
		case wxFONTENCODING_UTF16BE:
			wxFile::Write(BOM_UTF16BE, 2);
			m_bomOffset = 2;
			break;
		case wxFONTENCODING_UTF16LE:
			wxFile::Write(BOM_UTF16LE, 2);
			m_bomOffset = 2;
			break;
		case wxFONTENCODING_UTF32BE:
			wxFile::Write(BOM_UTF32LE, 4);
			m_bomOffset = 4;
			break;
		case wxFONTENCODING_UTF32LE:
			wxFile::Write(BOM_UTF32LE, 4);
			m_bomOffset = 4;
			break;
		default:
			m_bomOffset = 0;
			break;
	}
}


void wxUtfFile::DetermineConversion(wxFontEncoding encoding)
{
	switch (encoding)
	{
		case wxFONTENCODING_UTF8:
		case wxFONTENCODING_UTF16BE:
		case wxFONTENCODING_UTF16LE:
		case wxFONTENCODING_UTF32BE:
		case wxFONTENCODING_UTF32LE:
			// we know these
			m_encoding = encoding;
			break;
		default:
			m_encoding = wxFONTENCODING_SYSTEM;
			break;
	}

	if (m_encoding == wxFONTENCODING_UNICODE)
		m_conversion = 0;
	else
	{
		switch (m_encoding)
		{
			case wxFONTENCODING_SYSTEM:
				m_conversion = &wxConvLibc;
				break;
			case wxFONTENCODING_UTF8:
				m_conversion = &wxConvUTF8;
				break;
			case wxFONTENCODING_UTF16BE:
				m_conversion = &wxConvUTF16BE;
				break;
			case wxFONTENCODING_UTF16LE:
				m_conversion = &wxConvUTF16LE;
				break;
			case wxFONTENCODING_UTF32BE:
				m_conversion = &wxConvUTF32BE;
				break;
			case wxFONTENCODING_UTF32LE:
				m_conversion = &wxConvUTF32LE;
				break;
			default:
				break;
		}
	}
}


bool wxUtfFile::EvalBOM(wxFontEncoding encoding)
{
	// returns true, if BOM needs to be written.

	char bombuf[4] = "###";
	long len = wxFile::Read(bombuf, 4);

	if (!memcmp(bombuf, BOM_UTF32BE, 4))
	{
		encoding = wxFONTENCODING_UTF32BE;
		m_bomOffset = 4;
	}
	else if (!memcmp(bombuf, BOM_UTF32LE, 4))
	{
		encoding = wxFONTENCODING_UTF32LE;
		m_bomOffset = 4;
	}
	else if (!memcmp(bombuf, BOM_UTF8, 3))
	{
		encoding = wxFONTENCODING_UTF8;
		m_bomOffset = 3;
	}
	else if (!memcmp(bombuf, BOM_UTF16BE, 2))
	{
		encoding = wxFONTENCODING_UTF16BE;
		m_bomOffset = 2;
	}
	else if (!memcmp(bombuf, BOM_UTF16LE, 2))
	{
		encoding = wxFONTENCODING_UTF16LE;
		m_bomOffset = 2;
	}
	else
	{
		// no encoding was found.
		m_bomOffset = 0;
	}

	DetermineConversion(encoding);

	// if this file has length 0, BOM needs to be written because it's freshly created
	if (len == 0)
		return true;
	else if (len != m_bomOffset)
		Seek(0);

	return false;
}
