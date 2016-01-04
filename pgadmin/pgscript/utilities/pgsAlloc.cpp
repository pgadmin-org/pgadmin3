//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#include "pgAdmin3.h"

#if defined(PGSDEBUG)

#include "pgscript/utilities/pgsAlloc.h"
#include <wx/string.h>
#include <wx/log.h>

#undef new
#undef delete

pgsAlloc::pgsAlloc()
{

}

void pgsAlloc::add_malloc(const pgsMallocInfo &malloc_info)
{
	m_malloc_info[malloc_info.ptr] = malloc_info;
}

void pgsAlloc::rm_malloc(const void *ptr)
{
	if (m_malloc_info.size() != 0)
	{
		pgsMallocInfoMap::iterator it = m_malloc_info.find(ptr);
		if (it != m_malloc_info.end())
		{
			m_malloc_info.erase(it);
		}
	}
}

void pgsAlloc::dump()
{
	pgsMallocInfoMap::const_iterator it = m_malloc_info.begin();
	for (it = m_malloc_info.begin(); it != m_malloc_info.end(); it++)
	{
		const pgsMallocInfo &info = it->second;

		wxLogError(wxString() << info.filename << wxT(":")
		           << info.line_nb << wxT(" - ") << (int) info.ptr
		           << wxT(" of size ") << info.size);
	}
}

void *pgsAlloc::pmalloc(size_t size, const char *filename, size_t line_nb)
{
	// Nothing to do if there is nothing to allocate
	if (size == 0) return 0;

	void *ptr = 0;
	ptr = malloc(size);

	// Add allocation in the allocation map
	pgsMallocInfo malloc_info;
	malloc_info.ptr = ptr;
	malloc_info.size = size;
	malloc_info.filename = wxString(filename, wxConvFile);
	malloc_info.line_nb = line_nb;
	this->add_malloc(malloc_info);

	return ptr;
}

void pgsAlloc::pfree(void *ptr)
{
	// Remove information from the map if this piece of data exists
	this->rm_malloc(ptr);

	// Delete the data
	free(ptr);
}

pgsAlloc &pgsAlloc::instance()
{
	static pgsAlloc x;
	return x;
}

void *operator new(size_t size) throw (std::bad_alloc)
{
	return malloc(size);
}

void *operator new[](size_t size) throw (std::bad_alloc)
{
	return malloc(size);
}

void *operator new(size_t size, const char *filename, size_t line_nb)
throw (std::bad_alloc)
{
	return pgsAlloc::instance().pmalloc(size, filename, line_nb);
}

void *operator new[](size_t size, const char *filename, size_t line_nb)
throw (std::bad_alloc)
{
	return pgsAlloc::instance().pmalloc(size, filename, line_nb);
}

void operator delete(void *ptr) throw()
{
	pgsAlloc::instance().pfree(ptr);
}

void operator delete[](void *ptr) throw()
{
	pgsAlloc::instance().pfree(ptr);
}

#endif
