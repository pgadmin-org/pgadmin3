//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// registry.cpp - Windows Registry Reader for both 32 and 64 mode
//
//////////////////////////////////////////////////////////////////////////
#include "pgAdmin3.h"

#include "utils/registry.h"

#ifdef __WXMSW__

#define RESERVED        0
#define PGREG_SEPERATOR wxT("\\")

static struct stdkey
{
	HKEY         key;
	wxString name;
}
aStdHKeys[] =
{
	{HKEY_CLASSES_ROOT,   wxT("HKEY_CLASSES_ROOT")},
	{HKEY_CURRENT_USER,   wxT("HKEY_CURRENT_USER")},
	{HKEY_LOCAL_MACHINE,  wxT("HKEY_LOCAL_MACHINE") },
	{HKEY_USERS,          wxT("HKEY_USERS")},
	{HKEY_CURRENT_CONFIG, wxT("HKEY_CURRENT_CONFIG")}
};

#define NOS_STD_KEYS (size_t)(sizeof(aStdHKeys)/sizeof(stdkey))

pgRegKey::pgRegKey(HKEY root, const wxString &subkey, PGREGWOWMODE wowMode, PGREGACCESSMODE accessMode)
{
	Init(root, subkey, wowMode, accessMode);
}

pgRegKey::pgRegKey(const pgRegKey &parent, const wxString &key)
{
	wxString strKey;
	strKey << parent.m_strName << PGREG_SEPERATOR << key;

	Init(parent.m_hRoot, strKey, PGREG_WOW_DEFAULT, parent.m_accessMode);
	m_wowMode        = parent.m_wowMode;
}

void pgRegKey::Init(HKEY root, const wxString &subkey, PGREGWOWMODE wowMode, PGREGACCESSMODE accessMode)
{
	m_hRoot            = root;
	m_strName             = subkey;
	m_accessMode = accessMode;
	m_hKey             = NULL;
#ifdef __WIN64__
	m_wowMode = KEY_WOW64_64KEY;
#else
	m_wowMode = KEY_WOW64_32KEY;
#endif

	switch (wowMode)
	{
		case PGREG_WOW32:
#ifdef __WIN64__
			m_wowMode = KEY_WOW64_32KEY;
#endif // __WIN64__
			break;

		case PGREG_WOW64:
#ifndef __WIN64__
			// we can only access 64 bit registry under 64 bit platforms
			if (::wxIsPlatform64Bit())
				m_wowMode = KEY_WOW64_64KEY;
#endif // !__WIN64__
			break;
	}
}

pgRegKey *pgRegKey::OpenRegKey(HKEY root, const wxString &subkey, PGREGACCESSMODE accessmode, PGREGWOWMODE wowMode)
{
	wxString strKey = subkey;

	!subkey.IsEmpty() && subkey.EndsWith(wxT("\\"), &strKey);
	pgRegKey *tmpKey = new pgRegKey(root, strKey, wowMode, accessmode);

	HKEY tmpRegKey = 0;
	long nError = ::RegOpenKeyEx(root, (LPCTSTR)strKey, RESERVED,
	                             (accessmode == PGREG_READ ? KEY_READ : KEY_ALL_ACCESS) | tmpKey->m_wowMode,
	                             &tmpRegKey);

	if (nError != ERROR_SUCCESS)
	{
		delete(tmpKey);
		tmpKey = NULL;
	}
	else
		tmpKey->m_hKey = tmpRegKey;

	return tmpKey;
}

void pgRegKey::Close()
{
	if (m_hKey)
	{
		::RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

pgRegKey::~pgRegKey()
{
	Close();
}

bool pgRegKey::KeyExists(HKEY root, const wxString &subkey, PGREGWOWMODE wowMode)
{
	if (subkey.IsEmpty())
		return true;

#ifdef __WIN64__
	long keyWowMode = KEY_WOW64_64KEY;
#else
	long keyWowMode = KEY_WOW64_32KEY;
#endif

	switch (wowMode)
	{
		case PGREG_WOW32:
#ifdef __WIN64__
			keyWowMode = KEY_WOW64_32KEY;
#endif // __WIN64__
			break;

		case PGREG_WOW64:
#ifndef __WIN64__
			// we can only access 64 bit registry under 64 bit platforms
			if (::wxIsPlatform64Bit())
				keyWowMode = KEY_WOW64_64KEY;
#endif // ! __WIN64__
			break;
	}

	HKEY hkeyDummy;
	if (::RegOpenKeyEx(root, (LPCTSTR)subkey, RESERVED, KEY_READ | keyWowMode, &hkeyDummy) == ERROR_SUCCESS)
	{
		::RegCloseKey(hkeyDummy);
		return true;
	}
	return false;
}

bool pgRegKey::GetKeyInfo(size_t *pnSubKeys, size_t *pnMaxKeyLen, size_t *pnValues, size_t *pnMaxValueLen) const
{

	long nError = ::RegQueryInfoKey(m_hKey, NULL, NULL, RESERVED, (LPDWORD)pnSubKeys, (LPDWORD)pnMaxKeyLen, NULL,
	                                (LPDWORD)pnValues, (LPDWORD)pnMaxValueLen, NULL, NULL, NULL);

	if (nError != ERROR_SUCCESS)
		return false;
	return true;
}

wxString pgRegKey::ToString() const
{
	wxString str;
	size_t index = 0;
	for (; index < NOS_STD_KEYS; index++)
	{
		if (m_hRoot == aStdHKeys[index].key)
		{
			str = aStdHKeys[index].name;
			break;
		}
	}

	if (!m_strName.IsEmpty())
		str << wxT("\\") << m_strName;

	return str;
}

wxString pgRegKey::GetKeyName() const
{
	if (!m_strName.IsEmpty())
	{
		int index = m_strName.Find(wxT('\\'), true);
		return m_strName.SubString(index + 1, m_strName.Len());
	}
	return wxEmptyString;
}

bool pgRegKey::QueryValue(const wxString &strkey, LPDWORD pVal) const
{
	if (strkey.IsEmpty())
		return false;

	DWORD dwType, dwSize = sizeof(DWORD);
	unsigned char *pBuf = (unsigned char *)pVal;

	long nError = ::RegQueryValueEx(m_hKey, (LPCTSTR)strkey, RESERVED, &dwType, pBuf, &dwSize);

	if (nError != ERROR_SUCCESS)
		return false;

	if (dwType != REG_DWORD && dwType != REG_DWORD_BIG_ENDIAN)
		return false;
	return true;
}

bool pgRegKey::QueryValue(const wxString &strVal, wxString &sVal) const
{
	DWORD dwType, dwSize;

	long nError = RegQueryValueEx(m_hKey, (LPCTSTR)strVal, RESERVED, &dwType, NULL, &dwSize);

	if (nError == ERROR_SUCCESS)
	{
		sVal = wxT("");
		if (dwSize == 0)
			return true;

		wxChar *pBuf = (wxChar *)calloc(dwSize, sizeof(wxChar));
		nError = ::RegQueryValueEx(m_hKey, (LPCTSTR)strVal, RESERVED, &dwType, (LPBYTE)pBuf, &dwSize);

		DWORD index = 0;
		if (dwType == REG_EXPAND_SZ || dwType == REG_MULTI_SZ)
		{
			wxChar *actualValueStr = (wxChar *)calloc(dwSize, sizeof(wxChar));
			memcpy(actualValueStr, pBuf, dwSize * sizeof(wxChar));

			wxChar *curr_line;
			sVal = wxT("");
			do
			{
				DWORD dwExpSize = ::ExpandEnvironmentStrings((actualValueStr + index), NULL, 0);
				curr_line = (wxChar *)calloc(dwExpSize, sizeof(wxChar));

				::ExpandEnvironmentStrings(actualValueStr + index, curr_line, dwExpSize);

				if (index != 0)
					sVal << wxT("\n\r") << curr_line;
				else
					sVal << curr_line;
				index += (DWORD)wxStrlen(actualValueStr + index) + 1;
				free(curr_line);
			}
			while (actualValueStr[index] && dwType == REG_MULTI_SZ);

			free(actualValueStr);
		}
		else
			sVal = pBuf;

		free(pBuf);
		return true;
	}
	return false;
}

bool pgRegKey::QueryValue(const wxString &strVal, LPBYTE &pVal, DWORD &len) const
{
	DWORD dwType;
	if(RegQueryValueEx(m_hKey, (LPCTSTR)strVal,
	                   RESERVED, &dwType, pVal, &len) == ERROR_SUCCESS)
		return true;
	len = 0;
	return false;
}

bool pgRegKey::GetFirstValue(wxString &strkey, long &lIndex) const
{
	lIndex = 0;
	return GetNextValue(strkey, lIndex);
}

bool pgRegKey::GetNextValue(wxString &strval, long &lIndex) const
{
	if (lIndex < 0)
		return false;

	TCHAR szVal[1024];
	DWORD dwValLen = 1024;

	long nError = RegEnumValue(m_hKey, lIndex, szVal, &dwValLen, RESERVED, NULL, NULL, NULL);

	if (nError != ERROR_SUCCESS)
	{
		if (nError == ERROR_NO_MORE_ITEMS)
		{
			nError = ERROR_SUCCESS;
			lIndex = -1;
		}
		return false;
	}
	strval = szVal;
	lIndex++;
	return true;
}

bool pgRegKey::HasValue(const wxString &strval)
{
	long nRetVal = ::RegQueryValueEx(m_hKey, (LPCTSTR)strval, RESERVED, NULL, NULL, NULL);

	return nRetVal == ERROR_SUCCESS;
}

bool pgRegKey::GetFirstKey(pgRegKey *&pkey, long &lIndex) const
{
	lIndex = 0;
	return GetNextKey(pkey, lIndex);
}

bool pgRegKey::GetNextKey(pgRegKey *&pKey, long &lIndex) const
{
	pKey = NULL;

	if (lIndex < 0)
		return false;

	TCHAR szKey[1024];
	DWORD dwKeyLen = 1023;
	long nError = ::RegEnumKeyEx(m_hKey, lIndex, szKey, &dwKeyLen, NULL, NULL, NULL, NULL);

	if (nError != ERROR_SUCCESS)
	{
		lIndex = -1;
		return false;
	}
	lIndex++;
	wxString strSubKey = m_strName;
	strSubKey << wxT("\\") << szKey;

	pgRegKey *tmpKey = new pgRegKey(m_hRoot, (LPCTSTR)strSubKey, PGREG_WOW_DEFAULT, m_accessMode);
	tmpKey->m_wowMode = m_wowMode;

	HKEY tmpRegKey = 0;
	nError
	    = ::RegOpenKeyEx(m_hRoot, (LPCTSTR)strSubKey, RESERVED, (m_accessMode == PGREG_READ ? KEY_READ : KEY_ALL_ACCESS) | m_wowMode, &tmpRegKey);

	if (nError != ERROR_SUCCESS)
	{
		delete(tmpKey);
		tmpKey = NULL;
		pKey = NULL;

		return false;
	}
	else
		tmpKey->m_hKey = tmpRegKey;

	pKey = tmpKey;

	return true;
}

bool pgRegKey::HasKey(const wxString &strKey) const
{
	wxString strSubKey = m_strName;
	strSubKey << wxT("\\") << strKey;

	return pgRegKey::KeyExists(m_hRoot, strSubKey, m_wowMode == KEY_WOW64_64KEY ? PGREG_WOW64 : PGREG_WOW32) ;
}

DWORD pgRegKey::GetValueType(const wxString &key) const
{
	DWORD dwType;
	long nError = RegQueryValueEx((HKEY) m_hKey, (LPCTSTR)key, RESERVED,
	                              &dwType, NULL, NULL);

	if (nError != ERROR_SUCCESS)
		return REG_NONE;
	return dwType;
}

#endif // __WXMSW__
