/*
 * admin.c
 * miscellaneous administrative functions
 *
 * Copyright (c) 2002 - 2006, PostgreSQL Global Development Group
 *
 * Author: Andreas Pflug <pgadmin@pse-consulting.de>
 *
 * IDENTIFICATION
 *	  $PostgreSQL: $
 *
 */

#include "postgres.h"

#include <sys/file.h>
#include <signal.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>

#include "commands/dbcommands.h"
#include "miscadmin.h"
#include "storage/sinval.h"
#include "storage/fd.h"
#include "funcapi.h"
#include "catalog/pg_type.h"
#include "catalog/pg_tablespace.h"
#include "postmaster/syslogger.h"

extern DLLIMPORT char *DataDir;
extern DLLIMPORT char *Log_directory;
extern DLLIMPORT char *Log_filename;
extern DLLIMPORT pid_t PostmasterPid;

Datum pg_reload_conf(PG_FUNCTION_ARGS);
Datum pg_logfile_rotate(PG_FUNCTION_ARGS);
Datum pg_logdir_ls(PG_FUNCTION_ARGS);
Datum pg_postmaster_starttime(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pg_reload_conf);
PG_FUNCTION_INFO_V1(pg_logfile_rotate);
PG_FUNCTION_INFO_V1(pg_logdir_ls);
PG_FUNCTION_INFO_V1(pg_postmaster_starttime);
Datum
pg_reload_conf(PG_FUNCTION_ARGS)
{
	if (!superuser()) 
		ereport(ERROR,
				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
				 (errmsg("only superuser can signal the postmaster"))));

	if (kill(PostmasterPid, SIGHUP))
	{
		ereport(WARNING,
				(errmsg("failed to send signal to postmaster: %m")));

		PG_RETURN_INT32(0);
	}

	PG_RETURN_INT32(1);
}


typedef struct 
{
	char *location;
	DIR *dirdesc;
} directory_fctx;


/*
 * logfile handling functions
 */

#if ROTATE_SUPPORTED
/* not yet in backend */
Datum pg_logfile_rotate(PG_FUNCTION_ARGS)
{
	if (!superuser()) 
		ereport(ERROR,
				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
				 (errmsg("only superuser can issue a logfile rotation command"))));

    PG_RETURN_BOOL(LogFileRotate());
}

#endif

Datum pg_logdir_ls(PG_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	struct dirent *de;
	directory_fctx *fctx;

	if (!superuser()) 
		ereport(ERROR,
				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
				 (errmsg("only superuser can list the log directory"))));
	
	if (memcmp(Log_filename, "postgresql-%Y-%m-%d_%H%M%S.log", 30) != 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 (errmsg("the log_filename parameter must equal 'postgresql-%%Y-%%m-%%d_%%H%%M%%S.log'"))));

	if (SRF_IS_FIRSTCALL())
	{
		MemoryContext oldcontext;
		TupleDesc tupdesc;

		funcctx=SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

		fctx = palloc(sizeof(directory_fctx));
		if (is_absolute_path(Log_directory))
		    fctx->location = Log_directory;
		else
		{
			fctx->location = palloc(strlen(DataDir) + strlen(Log_directory) +2);
			sprintf(fctx->location, "%s/%s", DataDir, Log_directory);
		}
		tupdesc = CreateTemplateTupleDesc(2, false);
		TupleDescInitEntry(tupdesc, (AttrNumber) 1, "starttime",
						   TIMESTAMPOID, -1, 0);
		TupleDescInitEntry(tupdesc, (AttrNumber) 2, "filename",
						   TEXTOID, -1, 0);

		funcctx->attinmeta = TupleDescGetAttInMetadata(tupdesc);
		
		fctx->dirdesc = AllocateDir(fctx->location);

		if (!fctx->dirdesc)
		    ereport(ERROR,
					(errcode_for_file_access(),
					 errmsg("%s is not browsable: %m", fctx->location)));

		funcctx->user_fctx = fctx;
		MemoryContextSwitchTo(oldcontext);
	}

	funcctx=SRF_PERCALL_SETUP();
	fctx = (directory_fctx*) funcctx->user_fctx;

	if (!fctx->dirdesc)  /* not a readable directory  */
		SRF_RETURN_DONE(funcctx);

	while ((de = readdir(fctx->dirdesc)) != NULL)
	{
		char *values[2];
		HeapTuple tuple;
            
		char	   	*field[MAXDATEFIELDS];
		char		lowstr[MAXDATELEN + 1];
		int		dtype;
		int		nf, ftype[MAXDATEFIELDS];
		fsec_t		fsec;
		int		tz = 0;
		struct 		pg_tm date;

		/*
		 * Default format:
		 *        postgresql-YYYY-MM-DD_HHMMSS.log
		 */
		if (strlen(de->d_name) != 32
		    || memcmp(de->d_name, "postgresql-", 11)
			|| de->d_name[21] != '_'
			|| strcmp(de->d_name + 28, ".log"))
		      continue;

		values[1] = palloc(strlen(fctx->location) + strlen(de->d_name) + 2);
		sprintf(values[1], "%s/%s", fctx->location, de->d_name);

		values[0] = de->d_name + 11;       /* timestamp */
		values[0][17] = 0;

		/* parse and decode expected timestamp */

		/* The ParseDateTime signature changed in PostgreSQL 8.0.4. Because
		 * there is no way to check the PG version at build time at present
		 * we'll have to keep a manual condition compile here :-(
		 * Use the following line for < 8.0.4, otherwise, use the uncommented
		 * version below.
		 *
		 * if (ParseDateTime(values[0], lowstr, field, ftype, MAXDATEFIELDS, &nf))
		 *
		 */

		if (ParseDateTime(values[0], lowstr, sizeof(lowstr), field, ftype, MAXDATEFIELDS, &nf))
		    continue;

		if (DecodeDateTime(field, ftype, nf, &dtype, &date, &fsec, &tz))
		    continue;

		/* Seems the format fits the expected format; feed it into the tuple */

		tuple = BuildTupleFromCStrings(funcctx->attinmeta, values);

		SRF_RETURN_NEXT(funcctx, HeapTupleGetDatum(tuple));
	}

	FreeDir(fctx->dirdesc);
	SRF_RETURN_DONE(funcctx);
}


Datum pg_postmaster_starttime(PG_FUNCTION_ARGS)
{
    Timestamp result;
    
    if (!superuser()) 
            ereport(ERROR,
                            (errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
                             (errmsg("only superuser can query the postmaster starttime"))));

#ifdef WIN32
    {
	    struct pg_tm tm;
        extern DLLIMPORT HANDLE PostmasterHandle;
        FILETIME creationTime;
        FILETIME exitTime, kernelTime, userTime;
        SYSTEMTIME st;
    
        bool rc=GetProcessTimes(PostmasterHandle, &creationTime,
            &exitTime, &kernelTime, &userTime);
        if (!rc)
            PG_RETURN_NULL();
    
        FileTimeToSystemTime(&creationTime, &st);
    
        tm.tm_year = st.wYear;
        tm.tm_mon = st.wMonth;
        tm.tm_mday = st.wDay;
        tm.tm_hour = st.wHour;
        tm.tm_min = st.wMinute;
        tm.tm_sec = st.wSecond;

		if (tm2timestamp(&tm, 0, NULL, &result) != 0)
		{
			ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
            errmsg("postmaster timestamp out of range")));
		}
    }
#else
	{
		FILE *fp;
		char buffer[MAXPGPATH];
	    pg_time_t now = time(NULL);
		struct pg_tm *tm = pg_localtime(&now);
		double systemSeconds;
		long procJiffies;
		int i;

		tm->tm_year += 1900;
		tm->tm_mon += 1;

		if (tm2timestamp(tm, 0, NULL, &result) != 0)
		{
			ereport(ERROR,
            (errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
            errmsg("postmaster timestamp out of range")));
		}

		fp = fopen("/proc/uptime", "r");
		if (!fp)
		    ereport(ERROR,
					(errcode_for_file_access(), 
					 errmsg("could not open /proc/uptime: %m")));

		if (fscanf(fp, "%lf", &systemSeconds) != 1)
		    ereport(ERROR,
					(errcode_for_file_access(), 
					 errmsg("could not interpret /proc/uptime: %m")));

		fclose(fp);

		sprintf(buffer, "/proc/%u/stat", PostmasterPid);
		fp = fopen(buffer, "r");
		if (!fp)
		    ereport(ERROR,
					(errcode_for_file_access(), 
					 errmsg("could not open %s: %m", buffer)));


#define PROC_STAT_POS 22

		for (i=1 ; i < PROC_STAT_POS ; i++)
		{
		    char c;
			do
			{
				c = fgetc(fp);
			}
			while (c && c != ' ');
		}

		if (fscanf(fp, "%ld", &procJiffies) != 1)
		    ereport(ERROR,
					(errcode_for_file_access(), 
					 errmsg("could not interpret %s: %m", buffer)));

		fclose(fp);

#ifdef HAVE_INT64_TIMESTAMP
		result += (procJiffies/100. - systemSeconds) / INT64CONST(1000000);
#else
		result += (procJiffies/100. - systemSeconds);
#endif
	}
#endif


    PG_RETURN_TIMESTAMP(result);
}
