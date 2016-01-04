//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSALLOC_H
#define PGSALLOC_H

#if defined(PGSDEBUG)

#include <cstdlib> // malloc and free
#include <new> // std::bad_alloc
#include <wx/hashmap.h>

struct pgsMallocInfo
{
	const void *ptr;
	size_t size;
	wxString filename;
	size_t line_nb;
};

WX_DECLARE_VOIDPTR_HASH_MAP(pgsMallocInfo, pgsMallocInfoMap);

class pgsAlloc
{

protected:

	pgsAlloc();

	pgsMallocInfoMap m_malloc_info;

private:

	void add_malloc(const pgsMallocInfo &malloc_info);

	void rm_malloc(const void *ptr);

public:

	void *pmalloc(size_t size, const char *filename, size_t line_nb);

	void dump();

	void pfree(void *ptr);

	static pgsAlloc &instance();

};

void *operator new(size_t size) throw (std::bad_alloc);
void *operator new[](size_t size) throw (std::bad_alloc);
void *operator new(size_t size, const char *filename, size_t line_nb)
throw (std::bad_alloc);
void *operator new[](size_t size, const char *filename, size_t line_nb)
throw (std::bad_alloc);
void operator delete(void *ptr) throw ();
void operator delete[](void *ptr) throw ();

#define pnew new(__FILE__, __LINE__)

#else

#define pnew new

#endif // PGSDEBUG

#define pdelete(x)  if ((x) != 0) { delete   x; x = 0; }
#define pdeletea(x) if ((x) != 0) { delete[] x; x = 0; }

#endif /*PGSALLOC_H_*/
