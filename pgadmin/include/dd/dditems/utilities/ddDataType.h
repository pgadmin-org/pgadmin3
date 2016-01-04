//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
// ddDataType.h - data types related info for table use
//
//////////////////////////////////////////////////////////////////////////

#ifndef DDDATATYPES_H
#define DDDATATYPES_H

enum ddMenuColumn
{
	add_column = 321,
	del_column,
	ren_column,
	change_datatype
};

enum ddHiddenTypes
{
	DDTABLEFIGURE = 100,
	DDRELATIONSHIPFIGURE = 200,
	DDCOLUMNFIGURE = 300,
	DDCOLUMNOPTIONICON = 400,
	DDCOLUMNKINDICON = 500,
	DDTEXTTABLEITEMFIGURE = 600
};

enum ddDataType
{
	dt_null = 0,
	dt_serial,
	dt_boolean,
	dt_integer,
	dt_money,
	dt_varchar, //(n)
	dt_bigint,
	dt_bit, //(n)
	dt_bytea,
	dt_char, //(n)
	dt_cidr,
	dt_circle,
	dt_date,
	dt_doubleprec,
	dt_inet,
	dt_interval, //(n)
	dt_line,
	dt_Lseg,
	dt_macaddr,
	dt_numeric, //(p,s)
	dt_path,
	dt_point,
	dt_polygon,
	dt_real,
	dt_smallint,
	dt_text,
	dt_time,
	dt_timestamp,
	dt_varbit //(n)
};

static wxArrayString ddDatatypes;

#endif
