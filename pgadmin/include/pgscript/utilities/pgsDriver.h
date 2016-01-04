//////////////////////////////////////////////////////////////////////////
//
// pgScript - PostgreSQL Tools
//
// Copyright (C) 2002 - 2016, The pgAdmin Development Team
// This software is released under the PostgreSQL Licence
//
//////////////////////////////////////////////////////////////////////////


#ifndef PGSDRIVER_H_
#define PGSDRIVER_H_

#include "pgscript/pgScript.h"
#include "pgscript/utilities/pgsScanner.h"
#include "pgscript/parser.tab.hh"

// Forward declaration
class pgsContext;

// Forward declaration
class pgsProgram;

// Forward declaration
class pgsThread;

/** The pgscript namespace is used to encapsulate the three parser classes
 * pgscript::pgsParser, pgscript::pgsScanner and pgscript::pgsDriver */
namespace pgscript
{

/** The pgsDriver class brings together all components. It creates an instance of
 * the pgsParser and pgsScanner classes and connects them. Then the input stream is
 * fed into the scanner object and the parser gets its token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved. */
class pgsDriver
{
public:
	/// Construct a new parser driver context
	pgsDriver(class pgsContext &_context, class pgsProgram &_program,
	          class pgsThread &_thread);

	/// Destroy parser
	~pgsDriver();

	/// Enable debug output in the flex scanner
	bool trace_scanning;

	/// Enable debug output in the bison parser
	bool trace_parsing;

	/** Invoke the scanner and parser for a stream.
	 * @param in	input stream
	 * @return		true if successfully parsed
	 */
	bool parse_stream(std::istream &in);

	/** Invoke the scanner and parser on an input string.
	 * @param input	input string
	 * @return		true if successfully parsed
	 */
	bool parse_string(const wxString &input);

	/** Invoke the scanner and parser on a file. Use parse_stream with a
	 * std::ifstream if detection of file reading errors is required.
	 * @param filename	input file name
	 * @param conv  multi-byte string converter
	 * @return		true if successfully parsed
	 */
	bool parse_file(const wxString &filename, wxMBConv &conv = wxConvLocal);

	// To demonstrate pure handling of parse errors, instead of
	// simply dumping them on the standard error output, we will pass
	// them to the driver using the following two member functions.

	/** Error handling with associated line number. This can be modified to
	 * output the error e.g. to a dialog box. */
	void error(const class location &l, const wxString &m);

	/** Pointer to the current lexer instance, this is used to connect the
	 * parser to the scanner. It is used in the yylex macro. */
	class pgsScanner *lexer;

	/** Reference to the context filled during parsing of the expressions. */
	class pgsContext &context;

	/** Contains the list of statements to execute. */
	class pgsProgram &program;

	/** The thread in which this driver is included. */
	class pgsThread &thread;
};

} // namespace pgscript

#endif // PGSDRIVER_H_
