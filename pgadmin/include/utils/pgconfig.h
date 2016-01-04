//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// pgconfig.h - backend configuration classes
//
//////////////////////////////////////////////////////////////////////////


#ifndef __PGCONFIG
#define __PGCONFIG

#include <wx/hashmap.h>


class pgSet;
class pgConn;

class pgSettingItem;

class pgConfigLine
{
public:
	pgConfigLine()
	{
		item = 0;
		isComment = false;
	}

	pgConfigLine(pgConfigLine *line);

	bool Differs(pgConfigLine *line);
	wxString GetNewText();

	pgSettingItem *item;
	wxString value;
	wxString comment;
	bool isComment;
};

class pgConfigOrgLine : public pgConfigLine
{
public:
	pgConfigOrgLine(const wxString str) : pgConfigLine()
	{
		text = str;
	}
	pgConfigOrgLine(pgConfigLine *line);
	wxString GetNewText();

	wxString text;
	wxString commentIndent;
};

class pgHbaConfigLine
{
public:
	pgHbaConfigLine(const wxString &line = wxEmptyString);
	wxString GetText();
	const wxChar *GetConnectType();
	const wxChar *GetMethod();
	void Init(const wxString &line);

	enum pgHbaConnectType
	{
		PGC_LOCAL = 0,
		PGC_HOST,
		PGC_HOSTSSL,
		PGC_HOSTNOSSL,
		PGC_INVALIDCONF
	};
	enum pgHbaMethod
	{
		PGC_TRUST = 0,
		PGC_REJECT,
		PGC_MD5,
		PGC_CRYPT,
		PGC_PASSWORD,
		PGC_KRB4,
		PGC_KRB5,
		PGC_IDENT,
		PGC_PAM,
		PGC_LDAP,
		PGC_GSS,
		PGC_SSPI,
		PGC_CERT,
		PGC_PEER,
		PGC_RADIUS,
		PGC_INVALIDMETHOD
	};

	wxString text;
	wxString database, user, ipaddress, option;
	pgHbaConnectType connectType;
	pgHbaMethod method;

	long item;

	bool isValid;
	bool isComment;
	bool changed;
};

class pgPassConfigLine
{
public:
	pgPassConfigLine(const wxString &line = wxEmptyString);
	wxString GetText();
	void Init(const wxString &line);

	wxString text;
	wxString hostname, port, database, username, password;

	long item;

	bool isComment;
};


class pgSettingItem
{
public:
	enum pgConfigType
	{
		PGC_BOOL = 0,
		PGC_INT,
		PGC_REAL,
		PGC_STRING
	};

	enum pgConfigContext
	{
		PGC_INTERNAL = 0,
		PGC_POSTMASTER,
		PGC_SIGHUP,
		PGC_BACKEND,
		PGC_SUSET,
		PGC_USERLIMIT,
		PGC_USERSET,
		PGC_UNKNOWNCONTEXT
	};

	enum pgConfigSource
	{
		PGC_DEFAULT = 0,
		PGC_ENVIRONMENT,
		PGC_FILE,
		PGC_ARGV,
		PGC_UNPRIV,
		PGC_DATABASE,
		PGC_USER,
		PGC_CLIENT,
		PGC_OVERRIDE,
		PGC_INTERACTIVE,
		PGC_TEST,
		PGC_SESSION,
		PGC_UNKNOWNSOURCE
	};

	pgSettingItem()
	{
		orgLine = 0;
		newLine = 0;
		source = PGC_UNKNOWNSOURCE;
		context = PGC_UNKNOWNCONTEXT;
	}
	~pgSettingItem()
	{
		if (newLine) delete newLine;
	}

	void SetType(const wxString &str);
	void SetContext(const wxString &str);
	void SetSource(const wxString &str);
	wxString GetActiveValue();

	wxString name;
	wxString category;
	wxString short_desc;
	wxString extra_desc;
	wxString value;
	pgConfigType type;
	pgConfigContext context;
	pgConfigSource source;

	wxString min_val, max_val;

	pgConfigLine *newLine;
	pgConfigOrgLine *orgLine;
};



WX_DECLARE_HASH_MAP(wxString, pgSettingItem *, wxStringHash, wxStringEqual, pgSettingItemHashmap);
WX_DECLARE_HASH_MAP(wxString, wxArrayString *, wxStringHash, wxStringEqual, pgCategoryHashmap);

class pgSettingReader
{
public:
	virtual bool IsValid() = 0;
	virtual pgSettingItem *GetNextItem() = 0;
	virtual ~pgSettingReader() {}
};


class pgSettingFileReader : public pgSettingReader
{
	wxString columnNames;
	wxString buffer;
	wxChar *bp;

public:
	pgSettingFileReader(bool localized);
	~pgSettingFileReader();
	virtual bool IsValid()
	{
		return !buffer.IsEmpty();
	}
	virtual pgSettingItem *GetNextItem();
};


class pgSettingDbReader : public pgSettingReader
{
	pgSet *set;

public:
	pgSettingDbReader(pgConn *conn);
	~pgSettingDbReader();

	virtual bool IsValid()
	{
		return set != NULL;
	}
	virtual pgSettingItem *GetNextItem();
};

#endif
