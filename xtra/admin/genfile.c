/*-------------------------------------------------------------------------
 *
 * genfile.c
 *
 *
 * Copyright (c) 2004 - 2005, PostgreSQL Global Development Group
 * 
 * Author: Andreas Pflug <pgadmin@pse-consulting.de>
 *
 * IDENTIFICATION
 *	  $PostgreSQL: $
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "miscadmin.h"
#include "storage/fd.h"
#include "catalog/pg_type.h"
#include "funcapi.h"


#ifdef WIN32

#ifdef rename
#undef rename
#endif

#ifdef unlink
#undef unlink
#endif

#endif

extern DLLIMPORT char *DataDir;
extern DLLIMPORT char *Log_directory;
extern DLLIMPORT char *Log_filename_prefix;


Datum pg_file_stat(PG_FUNCTION_ARGS);
Datum pg_file_read(PG_FUNCTION_ARGS);
Datum pg_file_write(PG_FUNCTION_ARGS);
Datum pg_file_rename(PG_FUNCTION_ARGS);
Datum pg_file_unlink(PG_FUNCTION_ARGS);
Datum pg_dir_ls(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pg_file_stat);
PG_FUNCTION_INFO_V1(pg_file_read);
PG_FUNCTION_INFO_V1(pg_file_write);
PG_FUNCTION_INFO_V1(pg_file_rename);
PG_FUNCTION_INFO_V1(pg_file_unlink);
PG_FUNCTION_INFO_V1(pg_dir_ls);

typedef struct 
{
	char *location;
	DIR *dirdesc;
} directory_fctx;

/*-----------------------
 * some helper functions
 */

/*
 * Return an absolute path. Argument may be absolute or 
 * relative to the DataDir.
 */
static char *absClusterPath(text *arg, bool logAllowed)
{
	char *filename;
	int len=VARSIZE(arg) - VARHDRSZ;
	int dlen = strlen(DataDir);

	filename = palloc(len+1);
	memcpy(filename, VARDATA(arg), len);
	filename[len] = 0;

	if (strstr(filename, "..") != NULL)
	  ereport(ERROR,
			  (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
			   (errmsg("No .. allowed in filenames"))));
	
	if (is_absolute_path(filename))
	{
	    if (logAllowed && !strncmp(filename, Log_directory, strlen(Log_directory)))
		    return filename;
		if (strncmp(filename, DataDir, dlen))
		    ereport(ERROR,
					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
					 (errmsg("Absolute path not allowed"))));

		return filename;
	}
	else
	{
	    char *absname = palloc(dlen+len+2);
		sprintf(absname, "%s/%s", DataDir, filename);
		pfree(filename);
		return absname;
	}
}


/*
 * check for superuser, bark if not.
 */
static void
requireSuperuser(void)
{
	if (!superuser())
	    ereport(ERROR,
				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
				 (errmsg("only superuser may access generic file functions"))));
}



/* ------------------------------------
 * generic file handling functions
 */


Datum pg_file_stat(PG_FUNCTION_ARGS)
{
    AttInMetadata *attinmeta = NULL;
	char *	filename = absClusterPath(PG_GETARG_TEXT_P(0), true);
	struct stat fst;
	int64 length;
	char lenbuf[30];
	char cbuf[30], abuf[30], mbuf[30], dbuf[]="f";
	char *values[5] =
	   { lenbuf, cbuf, abuf, mbuf, dbuf };

	pg_time_t timestamp;
	HeapTuple tuple;

	if (attinmeta == NULL)
	{
		TupleDesc tupdesc = CreateTemplateTupleDesc(5, false);

		TupleDescInitEntry(tupdesc, (AttrNumber) 1, "length",
						   INT8OID, -1, 0);
		TupleDescInitEntry(tupdesc, (AttrNumber) 2, "ctime",
						   TIMESTAMPOID, -1, 0);
		TupleDescInitEntry(tupdesc, (AttrNumber) 3, "atime",
						   TIMESTAMPOID, -1, 0);
		TupleDescInitEntry(tupdesc, (AttrNumber) 4, "mtime",
						   TIMESTAMPOID, -1, 0);
		TupleDescInitEntry(tupdesc, (AttrNumber) 5, "isdir",
						   BOOLOID, -1, 0);

		attinmeta = TupleDescGetAttInMetadata(tupdesc);
	}

	if (stat(filename, &fst) < 0)
	{
		ereport(WARNING,
				(errcode_for_file_access(),
				 errmsg("could not stat file %s: %m", filename)));

		strcpy(lenbuf, "-1");
		strcpy(cbuf, "NULL");
		strcpy(abuf, "NULL");
		strcpy(mbuf, "NULL");
	}
	else
	{
		length = fst.st_size;  
		snprintf(lenbuf, 30, INT64_FORMAT, length);

		timestamp = fst.st_ctime;
		pg_strftime(cbuf, 30, "%F %T", pg_localtime(&timestamp));

		timestamp = fst.st_atime;
		pg_strftime(abuf, 30, "%F %T", pg_localtime(&timestamp));

		timestamp = fst.st_mtime;
		pg_strftime(mbuf, 30, "%F %T", pg_localtime(&timestamp));

		if (fst.st_mode & S_IFDIR)
		  dbuf[0] = 't';
	}
	tuple = BuildTupleFromCStrings(attinmeta, values);

	PG_RETURN_DATUM(HeapTupleGetDatum(tuple));
}


Datum pg_file_read(PG_FUNCTION_ARGS)
{
	size_t size;
	char *buf=0;
	size_t nbytes;
	int64 pos;
	FILE *f;
	char *filename;

	requireSuperuser();

	filename = absClusterPath(PG_GETARG_TEXT_P(0), true);
	pos = PG_GETARG_INT64(1);
	size = PG_GETARG_INT64(2);

	f = fopen(filename, "rb");
	if (!f)
	{
		ereport(ERROR,
				(errcode_for_file_access(),
				 errmsg("could not open file %s for reading: %m", filename)));
		PG_RETURN_NULL();
	}

	if (pos >= 0)
	    fseek(f, pos, SEEK_SET);
	else
	    fseek(f, pos, SEEK_END);


	buf = palloc(size + VARHDRSZ);

	nbytes = fread(VARDATA(buf), 1, size, f);
	if (nbytes < 0)
	{
		ereport(ERROR,
				(errcode_for_file_access(),
				 errmsg("could not read file %s: %m", filename)));
		PG_RETURN_NULL();
	}
	VARATT_SIZEP(buf) = nbytes + VARHDRSZ;
	fclose(f);

	PG_RETURN_TEXT_P(buf);
}


Datum pg_file_write(PG_FUNCTION_ARGS)
{
	FILE *f;
	char *filename;
	text *data;
	int64 count = 0;

	requireSuperuser();

	filename = absClusterPath(PG_GETARG_TEXT_P(0), false);
	data = PG_GETARG_TEXT_P(1);

	if (PG_ARGISNULL(2) || !PG_GETARG_BOOL(2))
	{
	    struct stat fst;
		if (stat(filename, &fst) >= 0)
		    ereport(ERROR,
					(ERRCODE_DUPLICATE_FILE,
					 errmsg("file %s exists", filename)));

	    f = fopen(filename, "wb");
	}
	else
	    f = fopen(filename, "ab");

	if (!f)
	{
		ereport(ERROR,
				(errcode_for_file_access(),
				 errmsg("could open file %s for writing: %m", filename)));
	}

	if (VARSIZE(data) != 0)
	{
		count = fwrite(VARDATA(data), 1, VARSIZE(data) - VARHDRSZ, f);

		if (count != VARSIZE(data) - VARHDRSZ)
		    ereport(ERROR,
					(errcode_for_file_access(),
					 errmsg("error writing file %s: %m", filename)));
	}
	fclose(f);

	PG_RETURN_INT64(count);
}


Datum pg_file_rename(PG_FUNCTION_ARGS)
{
    char *fn1, *fn2, *fn3;
	int rc;

	requireSuperuser();

	if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
		PG_RETURN_NULL();

	fn1=absClusterPath(PG_GETARG_TEXT_P(0), false);
	fn2=absClusterPath(PG_GETARG_TEXT_P(1), false);
	if (PG_ARGISNULL(2))
	    fn3=0;
	else
	    fn3=absClusterPath(PG_GETARG_TEXT_P(2), false);

	if (access(fn1, W_OK) < 0)
	{
		ereport(WARNING,
				(errcode_for_file_access(),
				 errmsg("file %s not accessible: %m", fn1)));

	    PG_RETURN_BOOL(false);
	}

	if (fn3 && access(fn2, W_OK) < 0)
	{
		ereport(WARNING,
				(errcode_for_file_access(),
				 errmsg("file %s not accessible: %m", fn2)));

	    PG_RETURN_BOOL(false);
	}


	rc = access(fn3 ? fn3 : fn2, 2);
	if (rc >= 0 || errno != ENOENT)
	{
		ereport(ERROR,
				(ERRCODE_DUPLICATE_FILE,
				 errmsg("cannot rename to target file %s", fn3 ? fn3 : fn2)));
	}
	
	if (fn3)
	{
	    if (rename(fn2, fn3) != 0)
		{
			ereport(ERROR,
					(errcode_for_file_access(),
					 errmsg("could not rename %s to %s: %m", fn2, fn3)));
		}
		if (rename(fn1, fn2) != 0)
		{
			ereport(WARNING,
					(errcode_for_file_access(),
					 errmsg("could not rename %s to %s: %m", fn1, fn2)));

			if (rename(fn3, fn2) != 0)
			{
				ereport(ERROR,
						(errcode_for_file_access(),
						 errmsg("could not rename %s back to %s: %m", fn3, fn2)));
			}
			else
			{
				ereport(ERROR,
						(ERRCODE_UNDEFINED_FILE,
						 errmsg("renaming %s to %s was reverted", fn2, fn3)));

			}
		}
	}
	else if (rename(fn1, fn2) != 0)
	{
			ereport(WARNING,
					(errcode_for_file_access(),
					 errmsg("renaming %s to %s %m", fn1, fn2)));
		ereport(ERROR,
				(errcode_for_file_access(),
				 errmsg("could not rename %s to %s: %m", fn1, fn2)));
	}

	PG_RETURN_BOOL(true);
}


Datum pg_file_unlink(PG_FUNCTION_ARGS)
{
    char *filename;

	requireSuperuser();

    filename = absClusterPath(PG_GETARG_TEXT_P(0), false);

	if (access(filename, W_OK) < 0)
	{
	    if (errno == ENOENT)
		    PG_RETURN_BOOL(false);
		else
		    ereport(ERROR,
					(errcode_for_file_access(),
					 errmsg("file %s not accessible: %m", filename)));

	}

	if (unlink(filename) < 0)
	{
		ereport(WARNING,
				(errcode_for_file_access(),
				 errmsg("could not unlink file %s: %m", filename)));

		PG_RETURN_BOOL(false);
	}
	PG_RETURN_BOOL(true);
}


Datum pg_dir_ls(PG_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	struct dirent *de;
	directory_fctx *fctx;

	requireSuperuser();

	if (SRF_IS_FIRSTCALL())
	{
		MemoryContext oldcontext;

		funcctx=SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

		fctx = palloc(sizeof(directory_fctx));
		fctx->location = absClusterPath(PG_GETARG_TEXT_P(0), false);

		fctx->dirdesc = AllocateDir(fctx->location);

		if (!fctx->dirdesc)
		    ereport(ERROR,
					(errcode_for_file_access(),
					 errmsg("%s is not browsable: %m", fctx->location)));

		if (PG_ARGISNULL(1) || !PG_GETARG_BOOL(1))
		{
			pfree(fctx->location);
			fctx->location = 0;
		}
		funcctx->user_fctx = fctx;
		MemoryContextSwitchTo(oldcontext);
	}

	funcctx=SRF_PERCALL_SETUP();
	fctx = (directory_fctx*) funcctx->user_fctx;

	if (!fctx->dirdesc)  /* not a readable directory  */
		SRF_RETURN_DONE(funcctx);

	while ((de = readdir(fctx->dirdesc)) != NULL)
	{
	    char *name;
		text *result;
		int len;
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
		    continue;
		if (fctx->location)
		{
			char *path=palloc(strlen(fctx->location) + strlen(de->d_name) +2);
			sprintf(path, "%s/%s", fctx->location, de->d_name);

			name = path;
		}
		else
		    name = de->d_name;


		len = strlen(name);
		result = palloc(len + VARHDRSZ);
		VARATT_SIZEP(result) = len + VARHDRSZ;
		memcpy(VARDATA(result), name, len);

		SRF_RETURN_NEXT(funcctx, PointerGetDatum(result));
	}

	FreeDir(fctx->dirdesc);
	SRF_RETURN_DONE(funcctx);
}


