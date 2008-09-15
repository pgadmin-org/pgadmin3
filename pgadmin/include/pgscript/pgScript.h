#ifndef PGSNITTEST_H_
#define PGSNITTEST_H_

/*** TYPEDEFS ***/

typedef unsigned char UCHAR;
typedef unsigned short int USHORT;

/*** DEFINES ***/

#define pgsReal true
#define pgsInt false

/*** PGADMIN ***/

#include "pgAdmin3.h"

/*** INCLUDES ***/

#if defined(HAVE_CONFIG_H) && defined(PGSCLI)
#include "config.h"
#endif

#include <wx/wx.h>

/*** OUTPUT ***/

#include <wx/txtstrm.h>
#define pgsOutputStream wxTextOutputStream

/*** LOGGING ***/

#include "utils/sysLogger.h"

/*** MEMORY LEAK DETECTION ***/

#include "pgscript/utilities/pgsAlloc.h"

#endif /*PGSNITTEST_H_*/
