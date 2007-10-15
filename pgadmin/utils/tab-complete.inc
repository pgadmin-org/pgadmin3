typedef struct SchemaQuery
{
	/*
	 * Name of catalog or catalogs to be queried, with alias, eg.
	 * "pg_catalog.pg_class c".  Note that "pg_namespace n" will be added.
	 */
	const char *catname;

	/*
	 * Selection condition --- only rows meeting this condition are candidates
	 * to display.	If catname mentions multiple tables, include the necessary
	 * join condition here.  For example, "c.relkind = 'r'". Write NULL (not
	 * an empty string) if not needed.
	 */
	const char *selcondition;

	/*
	 * Visibility condition --- which rows are visible without schema
	 * qualification?  For example, "pg_catalog.pg_table_is_visible(c.oid)".
	 */
	const char *viscondition;

	/*
	 * Namespace --- name of field to join to pg_namespace.oid. For example,
	 * "c.relnamespace".
	 */
	const char *namespace;

	/*
	 * Result --- the appropriately-quoted name to return, in the case of an
	 * unqualified name.  For example, "pg_catalog.quote_ident(c.relname)".
	 */
	const char *result;

	/*
	 * In some cases a different result must be used for qualified names.
	 * Enter that here, or write NULL if result can be used.
	 */
	const char *qualresult;
} SchemaQuery;


/*
 * Communication variables set by COMPLETE_WITH_FOO macros and then used by
 * the completion callback functions.  Ugly but there is no better way.
 */
static const char *completion_charp;	/* to pass a string */
static const char *const * completion_charpp;	/* to pass a list of strings */
static const char *completion_info_charp;		/* to pass a second string */

/*
 * Assembly instructions for schema queries
 */

static const SchemaQuery Query_for_list_of_aggregates = {
	/* catname */
	"pg_catalog.pg_proc p",
	/* selcondition */
	"p.proisagg",
	/* viscondition */
	"pg_catalog.pg_function_is_visible(p.oid)",
	/* namespace */
	"p.pronamespace",
	/* result */
	"pg_catalog.quote_ident(p.proname)",
	/* qualresult */
	NULL
};

static const SchemaQuery Query_for_list_of_datatypes = {
	/* catname */
	"pg_catalog.pg_type t",
	/* selcondition --- ignore table rowtypes and array types */
	"(t.typrelid = 0 "
	" OR (SELECT c.relkind = 'c' FROM pg_catalog.pg_class c WHERE c.oid = t.typrelid)) "
	"AND t.typname !~ '^_'",
	/* viscondition */
	"pg_catalog.pg_type_is_visible(t.oid)",
	/* namespace */
	"t.typnamespace",
	/* result */
	"pg_catalog.format_type(t.oid, NULL)",
	/* qualresult */
	"pg_catalog.quote_ident(t.typname)"
};

static const SchemaQuery Query_for_list_of_domains = {
	/* catname */
	"pg_catalog.pg_type t",
	/* selcondition */
	"t.typtype = 'd'",
	/* viscondition */
	"pg_catalog.pg_type_is_visible(t.oid)",
	/* namespace */
	"t.typnamespace",
	/* result */
	"pg_catalog.quote_ident(t.typname)",
	/* qualresult */
	NULL
};

static const SchemaQuery Query_for_list_of_functions = {
	/* catname */
	"pg_catalog.pg_proc p",
	/* selcondition */
	NULL,
	/* viscondition */
	"pg_catalog.pg_function_is_visible(p.oid)",
	/* namespace */
	"p.pronamespace",
	/* result */
	"pg_catalog.quote_ident(p.proname)",
	/* qualresult */
	NULL
};

static const SchemaQuery Query_for_list_of_indexes = {
	/* catname */
	"pg_catalog.pg_class c",
	/* selcondition */
	"c.relkind IN ('i')",
	/* viscondition */
	"pg_catalog.pg_table_is_visible(c.oid)",
	/* namespace */
	"c.relnamespace",
	/* result */
	"pg_catalog.quote_ident(c.relname)",
	/* qualresult */
	NULL
};

static const SchemaQuery Query_for_list_of_sequences = {
	/* catname */
	"pg_catalog.pg_class c",
	/* selcondition */
	"c.relkind IN ('S')",
	/* viscondition */
	"pg_catalog.pg_table_is_visible(c.oid)",
	/* namespace */
	"c.relnamespace",
	/* result */
	"pg_catalog.quote_ident(c.relname)",
	/* qualresult */
	NULL
};

static const SchemaQuery Query_for_list_of_tables = {
	/* catname */
	"pg_catalog.pg_class c",
	/* selcondition */
	"c.relkind IN ('r')",
	/* viscondition */
	"pg_catalog.pg_table_is_visible(c.oid)",
	/* namespace */
	"c.relnamespace",
	/* result */
	"pg_catalog.quote_ident(c.relname)",
	/* qualresult */
	NULL
};

static const SchemaQuery Query_for_list_of_tisv = {
	/* catname */
	"pg_catalog.pg_class c",
	/* selcondition */
	"c.relkind IN ('r', 'i', 'S', 'v')",
	/* viscondition */
	"pg_catalog.pg_table_is_visible(c.oid)",
	/* namespace */
	"c.relnamespace",
	/* result */
	"pg_catalog.quote_ident(c.relname)",
	/* qualresult */
	NULL
};

static const SchemaQuery Query_for_list_of_tsv = {
	/* catname */
	"pg_catalog.pg_class c",
	/* selcondition */
	"c.relkind IN ('r', 'S', 'v')",
	/* viscondition */
	"pg_catalog.pg_table_is_visible(c.oid)",
	/* namespace */
	"c.relnamespace",
	/* result */
	"pg_catalog.quote_ident(c.relname)",
	/* qualresult */
	NULL
};

static const SchemaQuery Query_for_list_of_views = {
	/* catname */
	"pg_catalog.pg_class c",
	/* selcondition */
	"c.relkind IN ('v')",
	/* viscondition */
	"pg_catalog.pg_table_is_visible(c.oid)",
	/* namespace */
	"c.relnamespace",
	/* result */
	"pg_catalog.quote_ident(c.relname)",
	/* qualresult */
	NULL
};


/*
 * Queries to get lists of names of various kinds of things, possibly
 * restricted to names matching a partially entered name.  In these queries,
 * %s will be replaced by the text entered so far (suitably escaped to
 * become a SQL literal string).  %d will be replaced by the length of the
 * string (in unescaped form).	A second %s, if present, will be replaced
 * by a suitably-escaped version of the string provided in
 * completion_info_charp.
 *
 * Beware that the allowed sequences of %s and %d are determined by
 * _complete_from_query().
 */

#define Query_for_list_of_attributes \
"SELECT pg_catalog.quote_ident(attname) "\
"  FROM pg_catalog.pg_attribute a, pg_catalog.pg_class c "\
" WHERE c.oid = a.attrelid "\
"   AND a.attnum > 0 "\
"   AND NOT a.attisdropped "\
"   AND substring(pg_catalog.quote_ident(attname),1,%d)='%s' "\
"   AND pg_catalog.quote_ident(relname)='%s' "\
"   AND pg_catalog.pg_table_is_visible(c.oid)"

#define Query_for_list_of_databases \
"SELECT pg_catalog.quote_ident(datname) FROM pg_catalog.pg_database "\
" WHERE substring(pg_catalog.quote_ident(datname),1,%d)='%s'"

#define Query_for_list_of_tablespaces \
"SELECT pg_catalog.quote_ident(spcname) FROM pg_catalog.pg_tablespace "\
" WHERE substring(pg_catalog.quote_ident(spcname),1,%d)='%s'"

#define Query_for_list_of_encodings \
" SELECT DISTINCT pg_catalog.pg_encoding_to_char(conforencoding) "\
"   FROM pg_catalog.pg_conversion "\
"  WHERE substring(pg_catalog.pg_encoding_to_char(conforencoding),1,%d)=UPPER('%s')"

#define Query_for_list_of_languages \
"SELECT pg_catalog.quote_ident(lanname) "\
"  FROM pg_language "\
" WHERE lanname != 'internal' "\
"   AND substring(pg_catalog.quote_ident(lanname),1,%d)='%s' "

#define Query_for_list_of_schemas \
"SELECT pg_catalog.quote_ident(nspname) FROM pg_catalog.pg_namespace "\
" WHERE substring(pg_catalog.quote_ident(nspname),1,%d)='%s'"

#define Query_for_list_of_set_vars \
"SELECT name FROM "\
" (SELECT pg_catalog.lower(name) AS name FROM pg_catalog.pg_settings "\
"  WHERE context IN ('user', 'superuser') "\
"  UNION ALL SELECT 'constraints' "\
"  UNION ALL SELECT 'transaction' "\
"  UNION ALL SELECT 'session' "\
"  UNION ALL SELECT 'role' "\
"  UNION ALL SELECT 'all') ss "\
" WHERE substring(name,1,%d)='%s'"

#define Query_for_list_of_show_vars \
"SELECT name FROM "\
" (SELECT pg_catalog.lower(name) AS name FROM pg_catalog.pg_settings "\
"  UNION ALL SELECT 'session authorization' "\
"  UNION ALL SELECT 'all') ss "\
" WHERE substring(name,1,%d)='%s'"

#define Query_for_list_of_system_relations \
"SELECT pg_catalog.quote_ident(relname) "\
"  FROM pg_catalog.pg_class c, pg_catalog.pg_namespace n "\
" WHERE c.relkind IN ('r', 'v', 's', 'S') "\
"   AND substring(pg_catalog.quote_ident(relname),1,%d)='%s' "\
"   AND c.relnamespace = n.oid "\
"   AND n.nspname = 'pg_catalog'"

#define Query_for_list_of_roles \
" SELECT pg_catalog.quote_ident(rolname) "\
"   FROM pg_catalog.pg_roles "\
"  WHERE substring(pg_catalog.quote_ident(rolname),1,%d)='%s'"

#define Query_for_list_of_grant_roles \
" SELECT pg_catalog.quote_ident(rolname) "\
"   FROM pg_catalog.pg_roles "\
"  WHERE substring(pg_catalog.quote_ident(rolname),1,%d)='%s'"\
" UNION ALL SELECT 'PUBLIC'"

/* the silly-looking length condition is just to eat up the current word */
#define Query_for_table_owning_index \
"SELECT pg_catalog.quote_ident(c1.relname) "\
"  FROM pg_catalog.pg_class c1, pg_catalog.pg_class c2, pg_catalog.pg_index i"\
" WHERE c1.oid=i.indrelid and i.indexrelid=c2.oid"\
"       and (%d = length('%s'))"\
"       and pg_catalog.quote_ident(c2.relname)='%s'"\
"       and pg_catalog.pg_table_is_visible(c2.oid)"

/* the silly-looking length condition is just to eat up the current word */
#define Query_for_index_of_table \
"SELECT pg_catalog.quote_ident(c2.relname) "\
"  FROM pg_catalog.pg_class c1, pg_catalog.pg_class c2, pg_catalog.pg_index i"\
" WHERE c1.oid=i.indrelid and i.indexrelid=c2.oid"\
"       and (%d = length('%s'))"\
"       and pg_catalog.quote_ident(c1.relname)='%s'"\
"       and pg_catalog.pg_table_is_visible(c2.oid)"

/* the silly-looking length condition is just to eat up the current word */
#define Query_for_list_of_tables_for_trigger \
"SELECT pg_catalog.quote_ident(relname) "\
"  FROM pg_catalog.pg_class"\
" WHERE (%d = length('%s'))"\
"   AND oid IN "\
"       (SELECT tgrelid FROM pg_catalog.pg_trigger "\
"         WHERE pg_catalog.quote_ident(tgname)='%s')"

/*
 * This is a list of all "things" in Pgsql, which can show up after CREATE or
 * DROP; and there is also a query to get a list of them.
 */

typedef struct
{
	const char *name;
	const char *query;			/* simple query, or NULL */
	const SchemaQuery *squery;	/* schema query, or NULL */
} pgsql_thing_t;

static const pgsql_thing_t words_after_create[] = {
	{"AGGREGATE", NULL, &Query_for_list_of_aggregates},
	{"CAST", NULL, NULL},		/* Casts have complex structures for names, so
								 * skip it */

	/*
	 * CREATE CONSTRAINT TRIGGER is not supported here because it is designed
	 * to be used only by pg_dump.
	 */
	{"CONVERSION", "SELECT pg_catalog.quote_ident(conname) FROM pg_catalog.pg_conversion WHERE substring(pg_catalog.quote_ident(conname),1,%d)='%s'"},
	{"DATABASE", Query_for_list_of_databases},
	{"DOMAIN", NULL, &Query_for_list_of_domains},
	{"FUNCTION", NULL, &Query_for_list_of_functions},
	{"GROUP", Query_for_list_of_roles},
	{"LANGUAGE", Query_for_list_of_languages},
	{"INDEX", NULL, &Query_for_list_of_indexes},
	{"OPERATOR", NULL, NULL},	/* Querying for this is probably not such a
								 * good idea. */
	{"ROLE", Query_for_list_of_roles},
	{"RULE", "SELECT pg_catalog.quote_ident(rulename) FROM pg_catalog.pg_rules WHERE substring(pg_catalog.quote_ident(rulename),1,%d)='%s'"},
	{"SCHEMA", Query_for_list_of_schemas},
	{"SEQUENCE", NULL, &Query_for_list_of_sequences},
	{"TABLE", NULL, &Query_for_list_of_tables},
	{"TABLESPACE", Query_for_list_of_tablespaces},
	{"TEMP", NULL, NULL},		/* for CREATE TEMP TABLE ... */
	{"TRIGGER", "SELECT pg_catalog.quote_ident(tgname) FROM pg_catalog.pg_trigger WHERE substring(pg_catalog.quote_ident(tgname),1,%d)='%s'"},
	{"TYPE", NULL, &Query_for_list_of_datatypes},
	{"UNIQUE", NULL, NULL},		/* for CREATE UNIQUE INDEX ... */
	{"USER", Query_for_list_of_roles},
	{"VIEW", NULL, &Query_for_list_of_views},
	{NULL, NULL, NULL}			/* end of list */
};


/* The completion function. Acc. to readline spec this gets passed the text
   entered to far and its start and end in the readline buffer. The return value
   is some partially obscure list format that can be generated by the readline
   libraries completion_matches() function, so we don't have to worry about it.
*/
static char * psql_completion(char *text, int start, int end, void *dbptr)
{
	/* This is the variable we'll return. */
	char *matches = NULL;

	/* These are going to contain some scannage of the input line. */
	char	   *prev_wd,
			   *prev2_wd,
			   *prev3_wd,
			   *prev4_wd,
			   *prev5_wd;

	static const char *const sql_commands[] = {
		"ABORT", "ALTER", "ANALYZE", "BEGIN", "CHECKPOINT", "CLOSE", "CLUSTER", "COMMENT",
		"COMMIT", "COPY", "CREATE", "DEALLOCATE", "DECLARE", "DELETE FROM", "DROP", "END", "EXECUTE",
		"EXPLAIN", "FETCH", "GRANT", "INSERT", "LISTEN", "LOAD", "LOCK", "MOVE", "NOTIFY",
		"PREPARE", "REINDEX", "RELEASE", "RESET", "REVOKE", "ROLLBACK", "SAVEPOINT",
		"SELECT", "SET", "SHOW", "START", "TRUNCATE", "UNLISTEN", "UPDATE", "VACUUM", NULL
	};

	static const char *const backslash_commands[] = {
		"\\a", "\\connect", "\\C", "\\cd", "\\copy", "\\copyright",
		"\\d", "\\da", "\\db", "\\dc", "\\dC", "\\dd", "\\dD", "\\df",
		"\\dg", "\\di", "\\dl", "\\dn", "\\do", "\\dp", "\\ds", "\\dS",
		"\\dt", "\\dT", "\\dv", "\\du",
		"\\e", "\\echo", "\\encoding",
		"\\f", "\\g", "\\h", "\\help", "\\H", "\\i", "\\l",
		"\\lo_import", "\\lo_export", "\\lo_list", "\\lo_unlink",
		"\\o", "\\p", "\\password", "\\pset", "\\q", "\\qecho", "\\r", "\\set", "\\t", "\\T",
		"\\timing", "\\unset", "\\x", "\\w", "\\z", "\\!", NULL
	};

	(void) end;					/* not used */

#ifdef HAVE_RL_COMPLETION_APPEND_CHARACTER
	rl_completion_append_character = ' ';
#endif

	/* Clear a few things. */
	completion_charp = NULL;
	completion_charpp = NULL;
	completion_info_charp = NULL;

	/*
	 * Scan the input line before our current position for the last four
	 * words. According to those we'll make some smart decisions on what the
	 * user is probably intending to type. TODO: Use strtokx() to do this.
	 */
	prev_wd = previous_word(start, 0);
	prev2_wd = previous_word(start, 1);
	prev3_wd = previous_word(start, 2);
	prev4_wd = previous_word(start, 3);
	prev5_wd = previous_word(start, 4);

	/* If a backslash command was started, continue */
	if (text[0] == '\\')
		COMPLETE_WITH_LIST(backslash_commands);

	/* If no previous word, suggest one of the basic sql commands */
	else if (!prev_wd)
		COMPLETE_WITH_LIST(sql_commands);

/* CREATE or DROP but not ALTER (TABLE|DOMAIN|GROUP) sth DROP */
	/* complete with something you can create or drop */
	else if (pg_strcasecmp(prev_wd, "CREATE") == 0 ||
			 (pg_strcasecmp(prev_wd, "DROP") == 0 &&
			  pg_strcasecmp(prev3_wd, "TABLE") != 0 &&
			  pg_strcasecmp(prev3_wd, "DOMAIN") != 0 &&
			  pg_strcasecmp(prev3_wd, "GROUP") != 0))
		matches = complete_create_command(text);

/* ALTER */

	/*
	 * complete with what you can alter (TABLE, GROUP, USER, ...) unless we're
	 * in ALTER TABLE sth ALTER
	 */
	else if (pg_strcasecmp(prev_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev3_wd, "TABLE") != 0)
	{
		static const char *const list_ALTER[] =
		{"AGGREGATE", "CONVERSION", "DATABASE", "DOMAIN", "FUNCTION",
			"GROUP", "INDEX", "LANGUAGE", "OPERATOR", "ROLE", "SCHEMA", "SEQUENCE", "TABLE",
		"TABLESPACE", "TRIGGER", "TYPE", "USER", NULL};

		COMPLETE_WITH_LIST(list_ALTER);
	}
	/* ALTER AGGREGATE,FUNCTION <name> */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 (pg_strcasecmp(prev2_wd, "AGGREGATE") == 0 ||
			  pg_strcasecmp(prev2_wd, "FUNCTION") == 0))
	{
		static const char *const list_ALTERAGG[] =
		{"OWNER TO", "RENAME TO", "SET SCHEMA", NULL};

		COMPLETE_WITH_LIST(list_ALTERAGG);
	}

	/* ALTER CONVERSION,SCHEMA <name> */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 (pg_strcasecmp(prev2_wd, "CONVERSION") == 0 ||
			  pg_strcasecmp(prev2_wd, "SCHEMA") == 0))
	{
		static const char *const list_ALTERGEN[] =
		{"OWNER TO", "RENAME TO", NULL};

		COMPLETE_WITH_LIST(list_ALTERGEN);
	}

	/* ALTER DATABASE <name> */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "DATABASE") == 0)
	{
		static const char *const list_ALTERDATABASE[] =
		{"RESET", "SET", "OWNER TO", "RENAME TO", "CONNECTION LIMIT", NULL};

		COMPLETE_WITH_LIST(list_ALTERDATABASE);
	}

	/* ALTER INDEX <name> */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "INDEX") == 0)
	{
		static const char *const list_ALTERINDEX[] =
		{"SET TABLESPACE", "OWNER TO", "RENAME TO", NULL};

		COMPLETE_WITH_LIST(list_ALTERINDEX);
	}

	/* ALTER LANGUAGE <name> */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "LANGUAGE") == 0)
		COMPLETE_WITH_CONST("RENAME TO");

	/* ALTER USER,ROLE <name> */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 (pg_strcasecmp(prev2_wd, "USER") == 0 ||
			  pg_strcasecmp(prev2_wd, "ROLE") == 0))
	{
		static const char *const list_ALTERUSER[] =
		{"ENCRYPTED", "UNENCRYPTED", "CREATEDB", "NOCREATEDB", "CREATEUSER",
			"NOCREATEUSER", "CREATEROLE", "NOCREATEROLE", "INHERIT", "NOINHERIT",
			"LOGIN", "NOLOGIN", "CONNECTION LIMIT", "VALID UNTIL", "RENAME TO",
		"SUPERUSER", "NOSUPERUSER", "SET", "RESET", NULL};

		COMPLETE_WITH_LIST(list_ALTERUSER);
	}

	/* complete ALTER USER,ROLE <name> ENCRYPTED,UNENCRYPTED with PASSWORD */
	else if (pg_strcasecmp(prev4_wd, "ALTER") == 0 &&
			 (pg_strcasecmp(prev3_wd, "ROLE") == 0 || pg_strcasecmp(prev3_wd, "USER") == 0) &&
			 (pg_strcasecmp(prev_wd, "ENCRYPTED") == 0 || pg_strcasecmp(prev_wd, "UNENCRYPTED") == 0))
	{
		COMPLETE_WITH_CONST("PASSWORD");
	}
	/* ALTER DOMAIN <name> */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "DOMAIN") == 0)
	{
		static const char *const list_ALTERDOMAIN[] =
		{"ADD", "DROP", "OWNER TO", "SET", NULL};

		COMPLETE_WITH_LIST(list_ALTERDOMAIN);
	}
	/* ALTER DOMAIN <sth> DROP */
	else if (pg_strcasecmp(prev4_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev3_wd, "DOMAIN") == 0 &&
			 pg_strcasecmp(prev_wd, "DROP") == 0)
	{
		static const char *const list_ALTERDOMAIN2[] =
		{"CONSTRAINT", "DEFAULT", "NOT NULL", NULL};

		COMPLETE_WITH_LIST(list_ALTERDOMAIN2);
	}
	/* ALTER DOMAIN <sth> SET */
	else if (pg_strcasecmp(prev4_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev3_wd, "DOMAIN") == 0 &&
			 pg_strcasecmp(prev_wd, "SET") == 0)
	{
		static const char *const list_ALTERDOMAIN3[] =
		{"DEFAULT", "NOT NULL", "SCHEMA", NULL};

		COMPLETE_WITH_LIST(list_ALTERDOMAIN3);
	}
	/* ALTER SEQUENCE <name> */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "SEQUENCE") == 0)
	{
		static const char *const list_ALTERSEQUENCE[] =
		{"INCREMENT", "MINVALUE", "MAXVALUE", "RESTART", "NO", "CACHE", "CYCLE",
		"SET SCHEMA", NULL};

		COMPLETE_WITH_LIST(list_ALTERSEQUENCE);
	}
	/* ALTER SEQUENCE <name> NO */
	else if (pg_strcasecmp(prev4_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev3_wd, "SEQUENCE") == 0 &&
			 pg_strcasecmp(prev_wd, "NO") == 0)
	{
		static const char *const list_ALTERSEQUENCE2[] =
		{"MINVALUE", "MAXVALUE", "CYCLE", NULL};

		COMPLETE_WITH_LIST(list_ALTERSEQUENCE2);
	}
	/* ALTER TRIGGER <name>, add ON */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "TRIGGER") == 0)
		COMPLETE_WITH_CONST("ON");

	else if (pg_strcasecmp(prev4_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev3_wd, "TRIGGER") == 0)
	{
		completion_info_charp = prev2_wd;
		COMPLETE_WITH_QUERY(Query_for_list_of_tables_for_trigger);
	}

	/*
	 * If we have ALTER TRIGGER <sth> ON, then add the correct tablename
	 */
	else if (pg_strcasecmp(prev4_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev3_wd, "TRIGGER") == 0 &&
			 pg_strcasecmp(prev_wd, "ON") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);

	/* ALTER TRIGGER <name> ON <name> */
	else if (pg_strcasecmp(prev4_wd, "TRIGGER") == 0 &&
			 pg_strcasecmp(prev2_wd, "ON") == 0)
		COMPLETE_WITH_CONST("RENAME TO");

	/*
	 * If we detect ALTER TABLE <name>, suggest either ADD, DROP, ALTER,
	 * RENAME, CLUSTER ON or OWNER
	 */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "TABLE") == 0)
	{
		static const char *const list_ALTER2[] =
		{"ADD", "ALTER", "CLUSTER ON", "DROP", "RENAME", "OWNER TO",
		"SET", NULL};

		COMPLETE_WITH_LIST(list_ALTER2);
	}
	/* If we have TABLE <sth> ALTER|RENAME, provide list of columns */
	else if (pg_strcasecmp(prev3_wd, "TABLE") == 0 &&
			 (pg_strcasecmp(prev_wd, "ALTER") == 0 ||
			  pg_strcasecmp(prev_wd, "RENAME") == 0))
		COMPLETE_WITH_ATTR(prev2_wd);

	/* ALTER TABLE xxx RENAME yyy */
	else if (pg_strcasecmp(prev4_wd, "TABLE") == 0 &&
			 pg_strcasecmp(prev2_wd, "RENAME") == 0 &&
			 pg_strcasecmp(prev_wd, "TO") != 0)
		COMPLETE_WITH_CONST("TO");

	/* If we have TABLE <sth> DROP, provide COLUMN or CONSTRAINT */
	else if (pg_strcasecmp(prev3_wd, "TABLE") == 0 &&
			 pg_strcasecmp(prev_wd, "DROP") == 0)
	{
		static const char *const list_TABLEDROP[] =
		{"COLUMN", "CONSTRAINT", NULL};

		COMPLETE_WITH_LIST(list_TABLEDROP);
	}
	/* If we have TABLE <sth> DROP COLUMN, provide list of columns */
	else if (pg_strcasecmp(prev4_wd, "TABLE") == 0 &&
			 pg_strcasecmp(prev2_wd, "DROP") == 0 &&
			 pg_strcasecmp(prev_wd, "COLUMN") == 0)
		COMPLETE_WITH_ATTR(prev3_wd);
	/* ALTER TABLE ALTER [COLUMN] <foo> */
	else if ((pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			  pg_strcasecmp(prev2_wd, "COLUMN") == 0) ||
			 (pg_strcasecmp(prev4_wd, "TABLE") == 0 &&
			  pg_strcasecmp(prev2_wd, "ALTER") == 0))
	{
		/* DROP ... does not work well yet */
		static const char *const list_COLUMNALTER[] =
		{"TYPE", "SET DEFAULT", "DROP DEFAULT", "SET NOT NULL",
		"DROP NOT NULL", "SET STATISTICS", "SET STORAGE", NULL};

		COMPLETE_WITH_LIST(list_COLUMNALTER);
	}
	else if (pg_strcasecmp(prev3_wd, "TABLE") == 0 &&
			 pg_strcasecmp(prev_wd, "CLUSTER") == 0)
		COMPLETE_WITH_CONST("ON");
	else if (pg_strcasecmp(prev4_wd, "TABLE") == 0 &&
			 pg_strcasecmp(prev2_wd, "CLUSTER") == 0 &&
			 pg_strcasecmp(prev_wd, "ON") == 0)
	{
		completion_info_charp = prev3_wd;
		COMPLETE_WITH_QUERY(Query_for_index_of_table);
	}
	/* If we have TABLE <sth> SET, provide WITHOUT,TABLESPACE and SCHEMA */
	else if (pg_strcasecmp(prev3_wd, "TABLE") == 0 &&
			 pg_strcasecmp(prev_wd, "SET") == 0)
	{
		static const char *const list_TABLESET[] =
		{"WITHOUT", "TABLESPACE", "SCHEMA", NULL};

		COMPLETE_WITH_LIST(list_TABLESET);
	}
	/* If we have TABLE <sth> SET TABLESPACE provide a list of tablespaces */
	else if (pg_strcasecmp(prev4_wd, "TABLE") == 0 &&
			 pg_strcasecmp(prev2_wd, "SET") == 0 &&
			 pg_strcasecmp(prev_wd, "TABLESPACE") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_tablespaces);
	/* If we have TABLE <sth> SET WITHOUT provide CLUSTER or OIDS */
	else if (pg_strcasecmp(prev4_wd, "TABLE") == 0 &&
			 pg_strcasecmp(prev2_wd, "SET") == 0 &&
			 pg_strcasecmp(prev_wd, "WITHOUT") == 0)
	{
		static const char *const list_TABLESET2[] =
		{"CLUSTER", "OIDS", NULL};

		COMPLETE_WITH_LIST(list_TABLESET2);
	}
	/* we have ALTER TABLESPACE, so suggest RENAME TO, OWNER TO */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "TABLESPACE") == 0)
	{
		static const char *const list_ALTERTSPC[] =
		{"RENAME TO", "OWNER TO", NULL};

		COMPLETE_WITH_LIST(list_ALTERTSPC);
	}
	/* complete ALTER TYPE <foo> with OWNER TO, SET SCHEMA */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "TYPE") == 0)
	{
		static const char *const list_ALTERTYPE[] =
		{"OWNER TO", "SET SCHEMA", NULL};

		COMPLETE_WITH_LIST(list_ALTERTYPE);
	}
	/* complete ALTER GROUP <foo> */
	else if (pg_strcasecmp(prev3_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "GROUP") == 0)
	{
		static const char *const list_ALTERGROUP[] =
		{"ADD USER", "DROP USER", "RENAME TO", NULL};

		COMPLETE_WITH_LIST(list_ALTERGROUP);
	}
	/* complete ALTER GROUP <foo> ADD|DROP with USER */
	else if (pg_strcasecmp(prev4_wd, "ALTER") == 0 &&
			 pg_strcasecmp(prev3_wd, "GROUP") == 0 &&
			 (pg_strcasecmp(prev_wd, "ADD") == 0 ||
			  pg_strcasecmp(prev_wd, "DROP") == 0))
		COMPLETE_WITH_CONST("USER");
	/* complete {ALTER} GROUP <foo> ADD|DROP USER with a user name */
	else if (pg_strcasecmp(prev4_wd, "GROUP") == 0 &&
			 (pg_strcasecmp(prev2_wd, "ADD") == 0 ||
			  pg_strcasecmp(prev2_wd, "DROP") == 0) &&
			 pg_strcasecmp(prev_wd, "USER") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_roles);

/* BEGIN, END, ABORT */
	else if (pg_strcasecmp(prev_wd, "BEGIN") == 0 ||
			 pg_strcasecmp(prev_wd, "END") == 0 ||
			 pg_strcasecmp(prev_wd, "ABORT") == 0)
	{
		static const char *const list_TRANS[] =
		{"WORK", "TRANSACTION", NULL};

		COMPLETE_WITH_LIST(list_TRANS);
	}
/* COMMIT */
	else if (pg_strcasecmp(prev_wd, "COMMIT") == 0)
	{
		static const char *const list_COMMIT[] =
		{"WORK", "TRANSACTION", "PREPARED", NULL};

		COMPLETE_WITH_LIST(list_COMMIT);
	}
/* RELEASE SAVEPOINT */
	else if (pg_strcasecmp(prev_wd, "RELEASE") == 0)
		COMPLETE_WITH_CONST("SAVEPOINT");
/* ROLLBACK*/
	else if (pg_strcasecmp(prev_wd, "ROLLBACK") == 0)
	{
		static const char *const list_TRANS[] =
		{"WORK", "TRANSACTION", "TO SAVEPOINT", "PREPARED", NULL};

		COMPLETE_WITH_LIST(list_TRANS);
	}
/* CLUSTER */

	/*
	 * If the previous word is CLUSTER and not without produce list of
	 * indexes.
	 */
	else if (pg_strcasecmp(prev_wd, "CLUSTER") == 0 &&
			 pg_strcasecmp(prev2_wd, "WITHOUT") != 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_indexes, NULL);
	/* If we have CLUSTER <sth>, then add "ON" */
	else if (pg_strcasecmp(prev2_wd, "CLUSTER") == 0 &&
			 pg_strcasecmp(prev_wd, "ON") != 0)
		COMPLETE_WITH_CONST("ON");

	/*
	 * If we have CLUSTER <sth> ON, then add the correct tablename as well.
	 */
	else if (pg_strcasecmp(prev3_wd, "CLUSTER") == 0 &&
			 pg_strcasecmp(prev_wd, "ON") == 0)
	{
		completion_info_charp = prev2_wd;
		COMPLETE_WITH_QUERY(Query_for_table_owning_index);
	}

/* COMMENT */
	else if (pg_strcasecmp(prev_wd, "COMMENT") == 0)
		COMPLETE_WITH_CONST("ON");
	else if (pg_strcasecmp(prev2_wd, "COMMENT") == 0 &&
			 pg_strcasecmp(prev_wd, "ON") == 0)
	{
		static const char *const list_COMMENT[] =
		{"CAST", "CONVERSION", "DATABASE", "INDEX", "LANGUAGE", "RULE", "SCHEMA",
			"SEQUENCE", "TABLE", "TYPE", "VIEW", "COLUMN", "AGGREGATE", "FUNCTION",
		"OPERATOR", "TRIGGER", "CONSTRAINT", "DOMAIN", "LARGE OBJECT", NULL};

		COMPLETE_WITH_LIST(list_COMMENT);
	}
	else if (pg_strcasecmp(prev4_wd, "COMMENT") == 0 &&
			 pg_strcasecmp(prev3_wd, "ON") == 0)
		COMPLETE_WITH_CONST("IS");

/* COPY */

	/*
	 * If we have COPY [BINARY] (which you'd have to type yourself), offer
	 * list of tables (Also cover the analogous backslash command)
	 */
	else if (pg_strcasecmp(prev_wd, "COPY") == 0 ||
			 pg_strcasecmp(prev_wd, "\\copy") == 0 ||
			 (pg_strcasecmp(prev2_wd, "COPY") == 0 &&
			  pg_strcasecmp(prev_wd, "BINARY") == 0))
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);
	/* If we have COPY|BINARY <sth>, complete it with "TO" or "FROM" */
	else if (pg_strcasecmp(prev2_wd, "COPY") == 0 ||
			 pg_strcasecmp(prev2_wd, "\\copy") == 0 ||
			 pg_strcasecmp(prev2_wd, "BINARY") == 0)
	{
		static const char *const list_FROMTO[] =
		{"FROM", "TO", NULL};

		COMPLETE_WITH_LIST(list_FROMTO);
	}
	/* If we have COPY|BINARY <sth> FROM|TO, complete with filename */
	else if ((pg_strcasecmp(prev3_wd, "COPY") == 0 ||
			  pg_strcasecmp(prev3_wd, "\\copy") == 0 ||
			  pg_strcasecmp(prev3_wd, "BINARY") == 0) &&
			 (pg_strcasecmp(prev_wd, "FROM") == 0 ||
			  pg_strcasecmp(prev_wd, "TO") == 0))
		matches = complete_filename();

	/* Handle COPY|BINARY <sth> FROM|TO filename */
	else if ((pg_strcasecmp(prev4_wd, "COPY") == 0 ||
			  pg_strcasecmp(prev4_wd, "\\copy") == 0 ||
			  pg_strcasecmp(prev4_wd, "BINARY") == 0) &&
			 (pg_strcasecmp(prev2_wd, "FROM") == 0 ||
			  pg_strcasecmp(prev2_wd, "TO") == 0))
	{
		static const char *const list_COPY[] =
		{"BINARY", "OIDS", "DELIMITER", "NULL", "CSV", NULL};

		COMPLETE_WITH_LIST(list_COPY);
	}

	/* Handle COPY|BINARY <sth> FROM|TO filename CSV */
	else if (pg_strcasecmp(prev_wd, "CSV") == 0 &&
			 (pg_strcasecmp(prev3_wd, "FROM") == 0 ||
			  pg_strcasecmp(prev3_wd, "TO") == 0))
	{
		static const char *const list_CSV[] =
		{"HEADER", "QUOTE", "ESCAPE", "FORCE QUOTE", NULL};

		COMPLETE_WITH_LIST(list_CSV);
	}

	/* CREATE DATABASE */
	else if (pg_strcasecmp(prev3_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev2_wd, "DATABASE") == 0)
	{
		static const char *const list_DATABASE[] =
		{"OWNER", "TEMPLATE", "ENCODING", "TABLESPACE", "CONNECTION LIMIT",
		NULL};

		COMPLETE_WITH_LIST(list_DATABASE);
	}

	/* CREATE INDEX */
	/* First off we complete CREATE UNIQUE with "INDEX" */
	else if (pg_strcasecmp(prev2_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev_wd, "UNIQUE") == 0)
		COMPLETE_WITH_CONST("INDEX");
	/* If we have CREATE|UNIQUE INDEX <sth>, then add "ON" */
	else if (pg_strcasecmp(prev2_wd, "INDEX") == 0 &&
			 (pg_strcasecmp(prev3_wd, "CREATE") == 0 ||
			  pg_strcasecmp(prev3_wd, "UNIQUE") == 0))
		COMPLETE_WITH_CONST("ON");
	/* Complete ... INDEX <name> ON with a list of tables  */
	else if (pg_strcasecmp(prev3_wd, "INDEX") == 0 &&
			 pg_strcasecmp(prev_wd, "ON") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);

	/*
	 * Complete INDEX <name> ON <table> with a list of table columns (which
	 * should really be in parens)
	 */
	else if (pg_strcasecmp(prev4_wd, "INDEX") == 0 &&
			 pg_strcasecmp(prev2_wd, "ON") == 0)
	{
		if (find_open_parenthesis(end))
			COMPLETE_WITH_ATTR(prev_wd);
		else
			COMPLETE_WITH_CONST("(");      
	}
	else if (pg_strcasecmp(prev5_wd, "INDEX") == 0 &&
			pg_strcasecmp(prev3_wd, "ON") == 0 &&
			pg_strcasecmp(prev_wd, "(") == 0)
		COMPLETE_WITH_ATTR(prev2_wd);
	/* same if you put in USING */
	else if (pg_strcasecmp(prev4_wd, "ON") == 0 &&
			 pg_strcasecmp(prev2_wd, "USING") == 0)
		COMPLETE_WITH_ATTR(prev3_wd);
	/* Complete USING with an index method */
	else if (pg_strcasecmp(prev_wd, "USING") == 0)
	{
		static const char *const index_mth[] =
		{"BTREE", "HASH", "GIST", NULL};

		COMPLETE_WITH_LIST(index_mth);
	}

/* CREATE RULE */
	/* Complete "CREATE RULE <sth>" with "AS" */
	else if (pg_strcasecmp(prev3_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev2_wd, "RULE") == 0)
		COMPLETE_WITH_CONST("AS");
	/* Complete "CREATE RULE <sth> AS with "ON" */
	else if (pg_strcasecmp(prev4_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev3_wd, "RULE") == 0 &&
			 pg_strcasecmp(prev_wd, "AS") == 0)
		COMPLETE_WITH_CONST("ON");
	/* Complete "RULE * AS ON" with SELECT|UPDATE|DELETE|INSERT */
	else if (pg_strcasecmp(prev4_wd, "RULE") == 0 &&
			 pg_strcasecmp(prev2_wd, "AS") == 0 &&
			 pg_strcasecmp(prev_wd, "ON") == 0)
	{
		static const char *const rule_events[] =
		{"SELECT", "UPDATE", "INSERT", "DELETE", NULL};

		COMPLETE_WITH_LIST(rule_events);
	}
	/* Complete "AS ON <sth with a 'T' :)>" with a "TO" */
	else if (pg_strcasecmp(prev3_wd, "AS") == 0 &&
			 pg_strcasecmp(prev2_wd, "ON") == 0 &&
			 (toupper((unsigned char) prev_wd[4]) == 'T' ||
			  toupper((unsigned char) prev_wd[5]) == 'T'))
		COMPLETE_WITH_CONST("TO");
	/* Complete "AS ON <sth> TO" with a table name */
	else if (pg_strcasecmp(prev4_wd, "AS") == 0 &&
			 pg_strcasecmp(prev3_wd, "ON") == 0 &&
			 pg_strcasecmp(prev_wd, "TO") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);

/* CREATE TABLE */
	/* Complete CREATE TEMP with "TABLE" */
	else if (pg_strcasecmp(prev2_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev_wd, "TEMP") == 0)
		COMPLETE_WITH_CONST("TABLE");

/* CREATE TABLESPACE */
	else if (pg_strcasecmp(prev3_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev2_wd, "TABLESPACE") == 0)
	{
		static const char *const list_CREATETABLESPACE[] =
		{"OWNER", "LOCATION", NULL};

		COMPLETE_WITH_LIST(list_CREATETABLESPACE);
	}
	/* Complete CREATE TABLESPACE name OWNER name with "LOCATION" */
	else if (pg_strcasecmp(prev5_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev4_wd, "TABLESPACE") == 0 &&
			 pg_strcasecmp(prev2_wd, "OWNER") == 0)
	{
		COMPLETE_WITH_CONST("LOCATION");
	}

/* CREATE TRIGGER */
	/* complete CREATE TRIGGER <name> with BEFORE,AFTER */
	else if (pg_strcasecmp(prev3_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev2_wd, "TRIGGER") == 0)
	{
		static const char *const list_CREATETRIGGER[] =
		{"BEFORE", "AFTER", NULL};

		COMPLETE_WITH_LIST(list_CREATETRIGGER);
	}
	/* complete CREATE TRIGGER <name> BEFORE,AFTER sth with OR,ON */
	else if (pg_strcasecmp(prev5_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev4_wd, "TRIGGER") == 0 &&
			 (pg_strcasecmp(prev2_wd, "BEFORE") == 0 ||
			  pg_strcasecmp(prev2_wd, "AFTER") == 0))
	{
		static const char *const list_CREATETRIGGER2[] =
		{"ON", "OR", NULL};

		COMPLETE_WITH_LIST(list_CREATETRIGGER2);
	}

/* CREATE ROLE,USER,GROUP */
	else if (pg_strcasecmp(prev3_wd, "CREATE") == 0 &&
			 (pg_strcasecmp(prev2_wd, "ROLE") == 0 ||
			  pg_strcasecmp(prev2_wd, "GROUP") == 0 || pg_strcasecmp(prev2_wd, "USER") == 0))
	{
		static const char *const list_CREATEROLE[] =
		{"ADMIN", "CONNECTION LIMIT", "CREATEDB", "CREATEROLE", "CREATEUSER",
			"ENCRYPTED", "IN", "INHERIT", "LOGIN", "NOINHERIT", "NOLOGIN", "NOCREATEDB",
			"NOCREATEROLE", "NOCREATEUSER", "NOSUPERUSER", "ROLE", "SUPERUSER", "SYSID",
		"UNENCRYPTED", NULL};

		COMPLETE_WITH_LIST(list_CREATEROLE);
	}

	/*
	 * complete CREATE ROLE,USER,GROUP <name> ENCRYPTED,UNENCRYPTED with
	 * PASSWORD
	 */
	else if (pg_strcasecmp(prev4_wd, "CREATE") == 0 &&
			 (pg_strcasecmp(prev3_wd, "ROLE") == 0 ||
			  pg_strcasecmp(prev3_wd, "GROUP") == 0 || pg_strcasecmp(prev3_wd, "USER") == 0) &&
			 (pg_strcasecmp(prev_wd, "ENCRYPTED") == 0 || pg_strcasecmp(prev_wd, "UNENCRYPTED") == 0))
	{
		COMPLETE_WITH_CONST("PASSWORD");
	}
	/* complete CREATE ROLE,USER,GROUP <name> IN with ROLE,GROUP */
	else if (pg_strcasecmp(prev4_wd, "CREATE") == 0 &&
			 (pg_strcasecmp(prev3_wd, "ROLE") == 0 ||
			  pg_strcasecmp(prev3_wd, "GROUP") == 0 || pg_strcasecmp(prev3_wd, "USER") == 0) &&
			 pg_strcasecmp(prev_wd, "IN") == 0)
	{
		static const char *const list_CREATEROLE3[] =
		{"GROUP", "ROLE", NULL};

		COMPLETE_WITH_LIST(list_CREATEROLE3);
	}

/* CREATE VIEW */
	/* Complete CREATE VIEW <name> with AS */
	else if (pg_strcasecmp(prev3_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev2_wd, "VIEW") == 0)
		COMPLETE_WITH_CONST("AS");
	/* Complete "CREATE VIEW <sth> AS with "SELECT" */
	else if (pg_strcasecmp(prev4_wd, "CREATE") == 0 &&
			 pg_strcasecmp(prev3_wd, "VIEW") == 0 &&
			 pg_strcasecmp(prev_wd, "AS") == 0)
		COMPLETE_WITH_CONST("SELECT");

/* DECLARE */
	else if (pg_strcasecmp(prev2_wd, "DECLARE") == 0)
	{
		static const char *const list_DECLARE[] =
		{"BINARY", "INSENSITIVE", "SCROLL", "NO SCROLL", "CURSOR", NULL};

		COMPLETE_WITH_LIST(list_DECLARE);
	}

	else if (pg_strcasecmp(prev_wd, "CURSOR") == 0)
	{
		static const char *const list_DECLARECURSOR[] =
		{"WITH HOLD", "WITHOUT HOLD", "FOR", NULL};

		COMPLETE_WITH_LIST(list_DECLARECURSOR);
	}


/* DELETE */

	/*
	 * Complete DELETE with FROM (only if the word before that is not "ON"
	 * (cf. rules) or "BEFORE" or "AFTER" (cf. triggers) or GRANT)
	 */
	else if (pg_strcasecmp(prev_wd, "DELETE") == 0 &&
			 !(pg_strcasecmp(prev2_wd, "ON") == 0 ||
			   pg_strcasecmp(prev2_wd, "GRANT") == 0 ||
			   pg_strcasecmp(prev2_wd, "BEFORE") == 0 ||
			   pg_strcasecmp(prev2_wd, "AFTER") == 0))
		COMPLETE_WITH_CONST("FROM");
	/* Complete DELETE FROM with a list of tables */
	else if (pg_strcasecmp(prev2_wd, "DELETE") == 0 &&
			 pg_strcasecmp(prev_wd, "FROM") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);
	/* Complete DELETE FROM <table> */
	else if (pg_strcasecmp(prev3_wd, "DELETE") == 0 &&
			 pg_strcasecmp(prev2_wd, "FROM") == 0)
	{
		static const char *const list_DELETE[] =
		{"USING", "WHERE", "SET", NULL};

		COMPLETE_WITH_LIST(list_DELETE);
	}
	/* XXX: implement tab completion for DELETE ... USING */

/* DROP (when not the previous word) */
	/* DROP AGGREGATE */
	else if (pg_strcasecmp(prev3_wd, "DROP") == 0 &&
			 pg_strcasecmp(prev2_wd, "AGGREGATE") == 0)
		COMPLETE_WITH_CONST("(");

	/* DROP object with CASCADE / RESTRICT */
	else if ((pg_strcasecmp(prev3_wd, "DROP") == 0 &&
			  (pg_strcasecmp(prev2_wd, "CONVERSION") == 0 ||
			   pg_strcasecmp(prev2_wd, "DOMAIN") == 0 ||
			   pg_strcasecmp(prev2_wd, "FUNCTION") == 0 ||
			   pg_strcasecmp(prev2_wd, "INDEX") == 0 ||
			   pg_strcasecmp(prev2_wd, "LANGUAGE") == 0 ||
			   pg_strcasecmp(prev2_wd, "SCHEMA") == 0 ||
			   pg_strcasecmp(prev2_wd, "SEQUENCE") == 0 ||
			   pg_strcasecmp(prev2_wd, "TABLE") == 0 ||
			   pg_strcasecmp(prev2_wd, "TYPE") == 0 ||
			   pg_strcasecmp(prev2_wd, "VIEW") == 0)) ||
			 (pg_strcasecmp(prev4_wd, "DROP") == 0 &&
			  pg_strcasecmp(prev3_wd, "AGGREGATE") == 0 &&
			  prev_wd[strlen(prev_wd) - 1] == ')'))
	{
		
		if ((pg_strcasecmp(prev3_wd, "DROP") == 0) && (pg_strcasecmp(prev2_wd, "FUNCTION") == 0))
                {
                        if (find_open_parenthesis(end))
			{
				static const char func_args_query[] = "select pg_catalog.oidvectortypes(proargtypes)||')' from pg_proc where proname='%s'";
                        	char *tmp_buf = malloc(strlen(func_args_query) + strlen(prev_wd));
                        	sprintf(tmp_buf, func_args_query, prev_wd);
                        	COMPLETE_WITH_QUERY(tmp_buf);
                        	free(tmp_buf);
			}
			else
			{
                        	COMPLETE_WITH_CONST("(");
			}
                }
                else
		{
			static const char *const list_DROPCR[] =
			{"CASCADE", "RESTRICT", NULL};
		
			COMPLETE_WITH_LIST(list_DROPCR);
		}
	}
	else if (pg_strcasecmp(prev4_wd, "DROP") == 0 &&
			pg_strcasecmp(prev3_wd, "FUNCTION") == 0 &&
			pg_strcasecmp(prev_wd, "(") == 0 )
	{
		static const char func_args_query[] = "select pg_catalog.oidvectortypes(proargtypes)||')' from pg_proc where proname='%s'";
		char *tmp_buf = malloc(strlen(func_args_query) + strlen(prev2_wd));
		sprintf(tmp_buf, func_args_query, prev2_wd);
		COMPLETE_WITH_QUERY(tmp_buf);
		free(tmp_buf);
	}



/* EXPLAIN */

	/*
	 * Complete EXPLAIN [ANALYZE] [VERBOSE] with list of EXPLAIN-able commands
	 */
	else if (pg_strcasecmp(prev_wd, "EXPLAIN") == 0)
	{
		static const char *const list_EXPLAIN[] =
		{"SELECT", "INSERT", "DELETE", "UPDATE", "DECLARE", "ANALYZE", "VERBOSE", NULL};

		COMPLETE_WITH_LIST(list_EXPLAIN);
	}
	else if (pg_strcasecmp(prev2_wd, "EXPLAIN") == 0 &&
			 pg_strcasecmp(prev_wd, "ANALYZE") == 0)
	{
		static const char *const list_EXPLAIN[] =
		{"SELECT", "INSERT", "DELETE", "UPDATE", "DECLARE", "VERBOSE", NULL};

		COMPLETE_WITH_LIST(list_EXPLAIN);
	}
	else if (pg_strcasecmp(prev_wd, "VERBOSE") == 0 &&
			 pg_strcasecmp(prev3_wd, "VACUUM") != 0 &&
			 pg_strcasecmp(prev4_wd, "VACUUM") != 0 &&
			 (pg_strcasecmp(prev2_wd, "ANALYZE") == 0 ||
			  pg_strcasecmp(prev2_wd, "EXPLAIN") == 0))
	{
		static const char *const list_EXPLAIN[] =
		{"SELECT", "INSERT", "DELETE", "UPDATE", "DECLARE", NULL};

		COMPLETE_WITH_LIST(list_EXPLAIN);
	}

/* FETCH && MOVE */
	/* Complete FETCH with one of FORWARD, BACKWARD, RELATIVE */
	else if (pg_strcasecmp(prev_wd, "FETCH") == 0 ||
			 pg_strcasecmp(prev_wd, "MOVE") == 0)
	{
		static const char *const list_FETCH1[] =
		{"ABSOLUTE", "BACKWARD", "FORWARD", "RELATIVE", NULL};

		COMPLETE_WITH_LIST(list_FETCH1);
	}
	/* Complete FETCH <sth> with one of ALL, NEXT, PRIOR */
	else if (pg_strcasecmp(prev2_wd, "FETCH") == 0 ||
			 pg_strcasecmp(prev2_wd, "MOVE") == 0)
	{
		static const char *const list_FETCH2[] =
		{"ALL", "NEXT", "PRIOR", NULL};

		COMPLETE_WITH_LIST(list_FETCH2);
	}

	/*
	 * Complete FETCH <sth1> <sth2> with "FROM" or "IN". These are equivalent,
	 * but we may as well tab-complete both: perhaps some users prefer one
	 * variant or the other.
	 */
	else if (pg_strcasecmp(prev3_wd, "FETCH") == 0 ||
			 pg_strcasecmp(prev3_wd, "MOVE") == 0)
	{
		static const char *const list_FROMIN[] =
		{"FROM", "IN", NULL};

		COMPLETE_WITH_LIST(list_FROMIN);
	}

/* GRANT && REVOKE*/
	/* Complete GRANT/REVOKE with a list of privileges */
	else if (pg_strcasecmp(prev_wd, "GRANT") == 0 ||
			 pg_strcasecmp(prev_wd, "REVOKE") == 0)
	{
		static const char *const list_privileg[] =
		{"SELECT", "INSERT", "UPDATE", "DELETE", "RULE", "REFERENCES",
		"TRIGGER", "CREATE", "TEMPORARY", "EXECUTE", "USAGE", "ALL", NULL};

		COMPLETE_WITH_LIST(list_privileg);
	}
	/* Complete GRANT/REVOKE <sth> with "ON" */
	else if (pg_strcasecmp(prev2_wd, "GRANT") == 0 ||
			 pg_strcasecmp(prev2_wd, "REVOKE") == 0)
		COMPLETE_WITH_CONST("ON");

	/*
	 * Complete GRANT/REVOKE <sth> ON with a list of tables, views, sequences,
	 * and indexes
	 *
	 * keywords DATABASE, FUNCTION, LANGUAGE, SCHEMA added to query result via
	 * UNION; seems to work intuitively
	 *
	 * Note: GRANT/REVOKE can get quite complex; tab-completion as implemented
	 * here will only work if the privilege list contains exactly one
	 * privilege
	 */
	else if ((pg_strcasecmp(prev3_wd, "GRANT") == 0 ||
			  pg_strcasecmp(prev3_wd, "REVOKE") == 0) &&
			 pg_strcasecmp(prev_wd, "ON") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tsv,
								   " UNION SELECT 'DATABASE'"
								   " UNION SELECT 'FUNCTION'"
								   " UNION SELECT 'LANGUAGE'"
								   " UNION SELECT 'SCHEMA'"
								   " UNION SELECT 'TABLESPACE'");

	/* Complete "GRANT/REVOKE * ON * " with "TO" */
	else if ((pg_strcasecmp(prev4_wd, "GRANT") == 0 ||
			  pg_strcasecmp(prev4_wd, "REVOKE") == 0) &&
			 pg_strcasecmp(prev2_wd, "ON") == 0)
	{
		if (pg_strcasecmp(prev_wd, "DATABASE") == 0)
			COMPLETE_WITH_QUERY(Query_for_list_of_databases);
		else if (pg_strcasecmp(prev_wd, "FUNCTION") == 0)
			COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_functions, NULL);
		else if (pg_strcasecmp(prev_wd, "LANGUAGE") == 0)
			COMPLETE_WITH_QUERY(Query_for_list_of_languages);
		else if (pg_strcasecmp(prev_wd, "SCHEMA") == 0)
			COMPLETE_WITH_QUERY(Query_for_list_of_schemas);
		else if (pg_strcasecmp(prev_wd, "TABLESPACE") == 0)
			COMPLETE_WITH_QUERY(Query_for_list_of_tablespaces);
		else if (pg_strcasecmp(prev4_wd, "GRANT") == 0)
			COMPLETE_WITH_CONST("TO");
		else
			COMPLETE_WITH_CONST("FROM");
	}

	/* Complete "GRANT/REVOKE * ON * TO/FROM" with username, GROUP, or PUBLIC */
	else if (pg_strcasecmp(prev3_wd, "ON") == 0 &&
			 ((pg_strcasecmp(prev5_wd, "GRANT") == 0 &&
			   pg_strcasecmp(prev_wd, "TO") == 0) ||
			  (pg_strcasecmp(prev5_wd, "REVOKE") == 0 &&
			   pg_strcasecmp(prev_wd, "FROM") == 0)))
		COMPLETE_WITH_QUERY(Query_for_list_of_grant_roles);

/* GROUP BY */
	else if (pg_strcasecmp(prev3_wd, "FROM") == 0 &&
			 pg_strcasecmp(prev_wd, "GROUP") == 0)
		COMPLETE_WITH_CONST("BY");

/* INSERT */
	/* Complete INSERT with "INTO" */
	else if (pg_strcasecmp(prev_wd, "INSERT") == 0)
		COMPLETE_WITH_CONST("INTO");
	/* Complete INSERT INTO with table names */
	else if (pg_strcasecmp(prev2_wd, "INSERT") == 0 &&
			 pg_strcasecmp(prev_wd, "INTO") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);
	/* Complete "INSERT INTO <table> (" with attribute names */
	else if (rl_line_buffer[start - 1] == '(' &&
			 pg_strcasecmp(prev3_wd, "INSERT") == 0 &&
			 pg_strcasecmp(prev2_wd, "INTO") == 0)
		COMPLETE_WITH_ATTR(prev_wd);

	/*
	 * Complete INSERT INTO <table> with "VALUES" or "SELECT" or "DEFAULT
	 * VALUES"
	 */
	else if (pg_strcasecmp(prev3_wd, "INSERT") == 0 &&
			 pg_strcasecmp(prev2_wd, "INTO") == 0)
	{
		static const char *const list_INSERT[] =
		{"DEFAULT VALUES", "SELECT", "VALUES", NULL};

		COMPLETE_WITH_LIST(list_INSERT);
	}
	/* Complete INSERT INTO <table> (attribs) with "VALUES" or "SELECT" */
	else if (pg_strcasecmp(prev4_wd, "INSERT") == 0 &&
			 pg_strcasecmp(prev3_wd, "INTO") == 0 &&
			 prev_wd[strlen(prev_wd) - 1] == ')')
	{
		static const char *const list_INSERT[] =
		{"SELECT", "VALUES", NULL};

		COMPLETE_WITH_LIST(list_INSERT);
	}

	/* Insert an open parenthesis after "VALUES" */
	else if (pg_strcasecmp(prev_wd, "VALUES") == 0 &&
			 pg_strcasecmp(prev2_wd, "DEFAULT") != 0)
		COMPLETE_WITH_CONST("(");

/* LOCK */
	/* Complete LOCK [TABLE] with a list of tables */
	else if (pg_strcasecmp(prev_wd, "LOCK") == 0 ||
			 (pg_strcasecmp(prev_wd, "TABLE") == 0 &&
			  pg_strcasecmp(prev2_wd, "LOCK") == 0))
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);

	/* For the following, handle the case of a single table only for now */

	/* Complete LOCK [TABLE] <table> with "IN" */
	else if ((pg_strcasecmp(prev2_wd, "LOCK") == 0 &&
			  pg_strcasecmp(prev_wd, "TABLE")) ||
			 (pg_strcasecmp(prev2_wd, "TABLE") == 0 &&
			  pg_strcasecmp(prev3_wd, "LOCK") == 0))
		COMPLETE_WITH_CONST("IN");

	/* Complete LOCK [TABLE] <table> IN with a lock mode */
	else if (pg_strcasecmp(prev_wd, "IN") == 0 &&
			 (pg_strcasecmp(prev3_wd, "LOCK") == 0 ||
			  (pg_strcasecmp(prev3_wd, "TABLE") == 0 &&
			   pg_strcasecmp(prev4_wd, "LOCK") == 0)))
	{
		static const char *const lock_modes[] =
		{"ACCESS SHARE MODE",
			"ROW SHARE MODE", "ROW EXCLUSIVE MODE",
			"SHARE UPDATE EXCLUSIVE MODE", "SHARE MODE",
			"SHARE ROW EXCLUSIVE MODE",
		"EXCLUSIVE MODE", "ACCESS EXCLUSIVE MODE", NULL};

		COMPLETE_WITH_LIST(lock_modes);
	}

/* NOTIFY */
	else if (pg_strcasecmp(prev_wd, "NOTIFY") == 0)
		COMPLETE_WITH_QUERY("SELECT pg_catalog.quote_ident(relname) FROM pg_catalog.pg_listener WHERE substring(pg_catalog.quote_ident(relname),1,%d)='%s'");

/* OWNER TO  - complete with available roles*/
	else if (pg_strcasecmp(prev2_wd, "OWNER") == 0 &&
			 pg_strcasecmp(prev_wd, "TO") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_roles);

/* ORDER BY */
	else if (pg_strcasecmp(prev3_wd, "FROM") == 0 &&
			 pg_strcasecmp(prev_wd, "ORDER") == 0)
		COMPLETE_WITH_CONST("BY");
	else if (pg_strcasecmp(prev4_wd, "FROM") == 0 &&
			 pg_strcasecmp(prev2_wd, "ORDER") == 0 &&
			 pg_strcasecmp(prev_wd, "BY") == 0)
		COMPLETE_WITH_ATTR(prev3_wd);

/* PREPARE xx AS */
	else if (pg_strcasecmp(prev_wd, "AS") == 0 &&
			 pg_strcasecmp(prev3_wd, "PREPARE") == 0)
	{
		static const char *const list_PREPARE[] =
		{"SELECT", "UPDATE", "INSERT", "DELETE", NULL};

		COMPLETE_WITH_LIST(list_PREPARE);
	}


/* REINDEX */
	else if (pg_strcasecmp(prev_wd, "REINDEX") == 0)
	{
		static const char *const list_REINDEX[] =
		{"TABLE", "INDEX", "SYSTEM", "DATABASE", NULL};

		COMPLETE_WITH_LIST(list_REINDEX);
	}
	else if (pg_strcasecmp(prev2_wd, "REINDEX") == 0)
	{
		if (pg_strcasecmp(prev_wd, "TABLE") == 0)
			COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);
		else if (pg_strcasecmp(prev_wd, "INDEX") == 0)
			COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_indexes, NULL);
		else if (pg_strcasecmp(prev_wd, "SYSTEM") == 0 ||
				 pg_strcasecmp(prev_wd, "DATABASE") == 0)
			COMPLETE_WITH_QUERY(Query_for_list_of_databases);
	}

/* SELECT */
	/* naah . . . */

/* SET, RESET, SHOW */
	/* Complete with a variable name */
	else if ((pg_strcasecmp(prev_wd, "SET") == 0 &&
			  pg_strcasecmp(prev3_wd, "UPDATE") != 0) ||
			 pg_strcasecmp(prev_wd, "RESET") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_set_vars);
	else if (pg_strcasecmp(prev_wd, "SHOW") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_show_vars);
	/* Complete "SET TRANSACTION" */
	else if ((pg_strcasecmp(prev2_wd, "SET") == 0 &&
			  pg_strcasecmp(prev_wd, "TRANSACTION") == 0)
			 || (pg_strcasecmp(prev2_wd, "START") == 0
				 && pg_strcasecmp(prev_wd, "TRANSACTION") == 0)
			 || (pg_strcasecmp(prev2_wd, "BEGIN") == 0
				 && pg_strcasecmp(prev_wd, "WORK") == 0)
			 || (pg_strcasecmp(prev2_wd, "BEGIN") == 0
				 && pg_strcasecmp(prev_wd, "TRANSACTION") == 0)
			 || (pg_strcasecmp(prev4_wd, "SESSION") == 0
				 && pg_strcasecmp(prev3_wd, "CHARACTERISTICS") == 0
				 && pg_strcasecmp(prev2_wd, "AS") == 0
				 && pg_strcasecmp(prev_wd, "TRANSACTION") == 0))
	{
		static const char *const my_list[] =
		{"ISOLATION LEVEL", "READ", NULL};

		COMPLETE_WITH_LIST(my_list);
	}
	else if ((pg_strcasecmp(prev3_wd, "SET") == 0
			  || pg_strcasecmp(prev3_wd, "BEGIN") == 0
			  || pg_strcasecmp(prev3_wd, "START") == 0
			  || (pg_strcasecmp(prev4_wd, "CHARACTERISTICS") == 0
				  && pg_strcasecmp(prev3_wd, "AS") == 0))
			 && (pg_strcasecmp(prev2_wd, "TRANSACTION") == 0
				 || pg_strcasecmp(prev2_wd, "WORK") == 0)
			 && pg_strcasecmp(prev_wd, "ISOLATION") == 0)
		COMPLETE_WITH_CONST("LEVEL");
	else if ((pg_strcasecmp(prev4_wd, "SET") == 0
			  || pg_strcasecmp(prev4_wd, "BEGIN") == 0
			  || pg_strcasecmp(prev4_wd, "START") == 0
			  || pg_strcasecmp(prev4_wd, "AS") == 0)
			 && (pg_strcasecmp(prev3_wd, "TRANSACTION") == 0
				 || pg_strcasecmp(prev3_wd, "WORK") == 0)
			 && pg_strcasecmp(prev2_wd, "ISOLATION") == 0
			 && pg_strcasecmp(prev_wd, "LEVEL") == 0)
	{
		static const char *const my_list[] =
		{"READ", "REPEATABLE", "SERIALIZABLE", NULL};

		COMPLETE_WITH_LIST(my_list);
	}
	else if ((pg_strcasecmp(prev4_wd, "TRANSACTION") == 0 ||
			  pg_strcasecmp(prev4_wd, "WORK") == 0) &&
			 pg_strcasecmp(prev3_wd, "ISOLATION") == 0 &&
			 pg_strcasecmp(prev2_wd, "LEVEL") == 0 &&
			 pg_strcasecmp(prev_wd, "READ") == 0)
	{
		static const char *const my_list[] =
		{"UNCOMMITTED", "COMMITTED", NULL};

		COMPLETE_WITH_LIST(my_list);
	}
	else if ((pg_strcasecmp(prev4_wd, "TRANSACTION") == 0 ||
			  pg_strcasecmp(prev4_wd, "WORK") == 0) &&
			 pg_strcasecmp(prev3_wd, "ISOLATION") == 0 &&
			 pg_strcasecmp(prev2_wd, "LEVEL") == 0 &&
			 pg_strcasecmp(prev_wd, "REPEATABLE") == 0)
		COMPLETE_WITH_CONST("READ");
	else if ((pg_strcasecmp(prev3_wd, "SET") == 0 ||
			  pg_strcasecmp(prev3_wd, "BEGIN") == 0 ||
			  pg_strcasecmp(prev3_wd, "START") == 0 ||
			  pg_strcasecmp(prev3_wd, "AS") == 0) &&
			 (pg_strcasecmp(prev2_wd, "TRANSACTION") == 0 ||
			  pg_strcasecmp(prev2_wd, "WORK") == 0) &&
			 pg_strcasecmp(prev_wd, "READ") == 0)
	{
		static const char *const my_list[] =
		{"ONLY", "WRITE", NULL};

		COMPLETE_WITH_LIST(my_list);
	}
	/* Complete SET CONSTRAINTS <foo> with DEFERRED|IMMEDIATE */
	else if (pg_strcasecmp(prev3_wd, "SET") == 0 &&
			 pg_strcasecmp(prev2_wd, "CONSTRAINTS") == 0)
	{
		static const char *const constraint_list[] =
		{"DEFERRED", "IMMEDIATE", NULL};

		COMPLETE_WITH_LIST(constraint_list);
	}
	/* Complete SET ROLE */
	else if (pg_strcasecmp(prev2_wd, "SET") == 0 &&
			 pg_strcasecmp(prev_wd, "ROLE") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_roles);
	/* Complete SET SESSION with AUTHORIZATION or CHARACTERISTICS... */
	else if (pg_strcasecmp(prev2_wd, "SET") == 0 &&
			 pg_strcasecmp(prev_wd, "SESSION") == 0)
	{
		static const char *const my_list[] =
		{"AUTHORIZATION", "CHARACTERISTICS AS TRANSACTION", NULL};

		COMPLETE_WITH_LIST(my_list);
	}
	/* Complete SET SESSION AUTHORIZATION with username */
	else if (pg_strcasecmp(prev3_wd, "SET") == 0
			 && pg_strcasecmp(prev2_wd, "SESSION") == 0
			 && pg_strcasecmp(prev_wd, "AUTHORIZATION") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_roles);
	/* Complete RESET SESSION with AUTHORIZATION */
	else if (pg_strcasecmp(prev2_wd, "RESET") == 0 &&
			 pg_strcasecmp(prev_wd, "SESSION") == 0)
		COMPLETE_WITH_CONST("AUTHORIZATION");
	/* Complete SET <var> with "TO" */
	else if (pg_strcasecmp(prev2_wd, "SET") == 0 &&
			 pg_strcasecmp(prev4_wd, "UPDATE") != 0 &&
			 pg_strcasecmp(prev_wd, "TABLESPACE") != 0 &&
			 pg_strcasecmp(prev4_wd, "DOMAIN") != 0)
		COMPLETE_WITH_CONST("TO");
	/* Suggest possible variable values */
	else if (pg_strcasecmp(prev3_wd, "SET") == 0 &&
			 (pg_strcasecmp(prev_wd, "TO") == 0 || strcmp(prev_wd, "=") == 0))
	{
		if (pg_strcasecmp(prev2_wd, "DateStyle") == 0)
		{
			static const char *const my_list[] =
			{"ISO", "SQL", "Postgres", "German",
				"YMD", "DMY", "MDY",
				"US", "European", "NonEuropean",
			"DEFAULT", NULL};

			COMPLETE_WITH_LIST(my_list);
		}
		else if (pg_strcasecmp(prev2_wd, "GEQO") == 0)
		{
			static const char *const my_list[] =
			{"ON", "OFF", "DEFAULT", NULL};

			COMPLETE_WITH_LIST(my_list);
		}
		else
		{
			static const char *const my_list[] =
			{"DEFAULT", NULL};

			COMPLETE_WITH_LIST(my_list);
		}
	}

/* START TRANSACTION */
	else if (pg_strcasecmp(prev_wd, "START") == 0)
		COMPLETE_WITH_CONST("TRANSACTION");

/* TRUNCATE */
	else if (pg_strcasecmp(prev_wd, "TRUNCATE") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);

/* UNLISTEN */
	else if (pg_strcasecmp(prev_wd, "UNLISTEN") == 0)
		COMPLETE_WITH_QUERY("SELECT pg_catalog.quote_ident(relname) FROM pg_catalog.pg_listener WHERE substring(pg_catalog.quote_ident(relname),1,%d)='%s' UNION SELECT '*'");

/* UPDATE */
	/* If prev. word is UPDATE suggest a list of tables */
	else if (pg_strcasecmp(prev_wd, "UPDATE") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);
	/* Complete UPDATE <table> with "SET" */
	else if (pg_strcasecmp(prev2_wd, "UPDATE") == 0)
		COMPLETE_WITH_CONST("SET");

	/*
	 * If the previous word is SET (and it wasn't caught above as the _first_
	 * word) the word before it was (hopefully) a table name and we'll now
	 * make a list of attributes.
	 */
	else if (pg_strcasecmp(prev_wd, "SET") == 0)
		COMPLETE_WITH_ATTR(prev2_wd);

/* UPDATE xx SET yy = */
	else if (pg_strcasecmp(prev2_wd, "SET") == 0 &&
			 pg_strcasecmp(prev4_wd, "UPDATE") == 0)
		COMPLETE_WITH_CONST("=");

/*
 * VACUUM [ FULL | FREEZE ] [ VERBOSE ] [ table ]
 * VACUUM [ FULL | FREEZE ] [ VERBOSE ] ANALYZE [ table [ (column [, ...] ) ] ]
 */
	else if (pg_strcasecmp(prev_wd, "VACUUM") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables,
								   " UNION SELECT 'FULL'"
								   " UNION SELECT 'FREEZE'"
								   " UNION SELECT 'ANALYZE'"
								   " UNION SELECT 'VERBOSE'");
	else if (pg_strcasecmp(prev2_wd, "VACUUM") == 0 &&
			 (pg_strcasecmp(prev_wd, "FULL") == 0 ||
			  pg_strcasecmp(prev_wd, "FREEZE") == 0))
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables,
								   " UNION SELECT 'ANALYZE'"
								   " UNION SELECT 'VERBOSE'");
	else if (pg_strcasecmp(prev3_wd, "VACUUM") == 0 &&
			 pg_strcasecmp(prev_wd, "ANALYZE") == 0 &&
			 (pg_strcasecmp(prev2_wd, "FULL") == 0 ||
			  pg_strcasecmp(prev2_wd, "FREEZE") == 0))
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables,
								   " UNION SELECT 'VERBOSE'");
	else if (pg_strcasecmp(prev3_wd, "VACUUM") == 0 &&
			 pg_strcasecmp(prev_wd, "VERBOSE") == 0 &&
			 (pg_strcasecmp(prev2_wd, "FULL") == 0 ||
			  pg_strcasecmp(prev2_wd, "FREEZE") == 0))
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables,
								   " UNION SELECT 'ANALYZE'");
	else if (pg_strcasecmp(prev2_wd, "VACUUM") == 0 &&
			 pg_strcasecmp(prev_wd, "VERBOSE") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables,
								   " UNION SELECT 'ANALYZE'");
	else if (pg_strcasecmp(prev2_wd, "VACUUM") == 0 &&
			 pg_strcasecmp(prev_wd, "ANALYZE") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables,
								   " UNION SELECT 'VERBOSE'");
	else if ((pg_strcasecmp(prev_wd, "ANALYZE") == 0 &&
			  pg_strcasecmp(prev2_wd, "VERBOSE") == 0) ||
			 (pg_strcasecmp(prev_wd, "VERBOSE") == 0 &&
			  pg_strcasecmp(prev2_wd, "ANALYZE") == 0))
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);

/* ANALYZE */
	/* If the previous word is ANALYZE, produce list of tables */
	else if (pg_strcasecmp(prev_wd, "ANALYZE") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);

/* WHERE */
	/* Simple case of the word before the where being the table name */
	else if (pg_strcasecmp(prev_wd, "WHERE") == 0)
		COMPLETE_WITH_ATTR(prev2_wd);

/* ... FROM ... */
/* TODO: also include SRF ? */
	else if (pg_strcasecmp(prev_wd, "FROM") == 0 &&
			 pg_strcasecmp(prev3_wd, "COPY") != 0 &&
			 pg_strcasecmp(prev3_wd, "\\copy") != 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tsv, NULL);


/* Backslash commands */
/* TODO:  \dc \dd \dl */
	else if (strcmp(prev_wd, "\\connect") == 0 || strcmp(prev_wd, "\\c") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_databases);
	else if (strcmp(prev_wd, "\\d") == 0 || strcmp(prev_wd, "\\d+") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tisv, NULL);
	else if (strcmp(prev_wd, "\\da") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_aggregates, NULL);
	else if (strcmp(prev_wd, "\\db") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_tablespaces);
	else if (strcmp(prev_wd, "\\dD") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_domains, NULL);
	else if (strcmp(prev_wd, "\\df") == 0 || strcmp(prev_wd, "\\df+") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_functions, NULL);
	else if (strcmp(prev_wd, "\\di") == 0 || strcmp(prev_wd, "\\di+") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_indexes, NULL);
	else if (strcmp(prev_wd, "\\dn") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_schemas);
	else if (strcmp(prev_wd, "\\dp") == 0 || strcmp(prev_wd, "\\z") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tsv, NULL);
	else if (strcmp(prev_wd, "\\ds") == 0 || strcmp(prev_wd, "\\ds+") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_sequences, NULL);
	else if (strcmp(prev_wd, "\\dS") == 0 || strcmp(prev_wd, "\\dS+") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_system_relations);
	else if (strcmp(prev_wd, "\\dt") == 0 || strcmp(prev_wd, "\\dt+") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_tables, NULL);
	else if (strcmp(prev_wd, "\\dT") == 0 || strcmp(prev_wd, "\\dT+") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_datatypes, NULL);
	else if (strcmp(prev_wd, "\\du") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_roles);
	else if (strcmp(prev_wd, "\\dv") == 0 || strcmp(prev_wd, "\\dv+") == 0)
		COMPLETE_WITH_SCHEMA_QUERY(Query_for_list_of_views, NULL);
	else if (strcmp(prev_wd, "\\encoding") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_encodings);
	else if (strcmp(prev_wd, "\\h") == 0 || strcmp(prev_wd, "\\help") == 0)
		COMPLETE_WITH_LIST(sql_commands);
	else if (strcmp(prev_wd, "\\password") == 0)
		COMPLETE_WITH_QUERY(Query_for_list_of_roles);
	else if (strcmp(prev_wd, "\\pset") == 0)
	{
		static const char *const my_list[] =
		{"format", "border", "expanded",
			"null", "fieldsep", "tuples_only", "title", "tableattr", "pager",
		"recordsep", NULL};

		COMPLETE_WITH_LIST(my_list);
	}
	else if (strcmp(prev_wd, "\\cd") == 0 ||
			 strcmp(prev_wd, "\\e") == 0 || strcmp(prev_wd, "\\edit") == 0 ||
			 strcmp(prev_wd, "\\g") == 0 ||
		  strcmp(prev_wd, "\\i") == 0 || strcmp(prev_wd, "\\include") == 0 ||
			 strcmp(prev_wd, "\\o") == 0 || strcmp(prev_wd, "\\out") == 0 ||
			 strcmp(prev_wd, "\\s") == 0 ||
			 strcmp(prev_wd, "\\w") == 0 || strcmp(prev_wd, "\\write") == 0
		)
		matches = complete_filename();


	/*
	 * Finally, we look through the list of "things", such as TABLE, INDEX and
	 * check if that was the previous word. If so, execute the query to get a
	 * list of them.
	 */
	else
	{
		int			i;

		for (i = 0; words_after_create[i].name; i++)
		{
			if (pg_strcasecmp(prev_wd, words_after_create[i].name) == 0)
			{
				if (words_after_create[i].query)
					COMPLETE_WITH_QUERY(words_after_create[i].query);
				else if (words_after_create[i].squery)
					COMPLETE_WITH_SCHEMA_QUERY(*words_after_create[i].squery,
											   NULL);
				break;
			}
		}
	}

	/*
	 * If we still don't have anything to match we have to fabricate some sort
	 * of default list. If we were to just return NULL, readline automatically
	 * attempts filename completion, and that's usually no good.
	 */
	if (matches == NULL)
	{
		COMPLETE_WITH_CONST("");
#ifdef HAVE_RL_COMPLETION_APPEND_CHARACTER
		rl_completion_append_character = '\0';
#endif
	}

	/* free storage */
	free(prev_wd);
	free(prev2_wd);
	free(prev3_wd);
	free(prev4_wd);
	free(prev5_wd);

	/* Return our Grand List O' Matches */
	return matches;
}





