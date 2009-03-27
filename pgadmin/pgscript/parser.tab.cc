#include "pgAdmin3.h"
#if _MSC_VER > 1000
#pragma warning(disable: 4800)
#endif

/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++

   Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

// Take the name prefix into account.
#define yylex   pgscriptlex

#include "pgscript/parser.tab.hh"

/* User implementation prologue.  */
#line 251 "pgscript/pgsParser.yy"


#include "pgscript/utilities/pgsDriver.h"
#include "pgscript/utilities/pgsScanner.h"

/* This "connects" the bison parser in the driver to the flex scanner class
 * object. It defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef  yylex
#define yylex driver.lexer->lex



/* Line 317 of lalr1.cc.  */
#line 57 "pgscript/parser.tab.cc"

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG							\
  for (bool yydebugcond_ = yydebug_; yydebugcond_; yydebugcond_ = false)	\
    (*yycdebug_)

/* Enable debugging if requested.  */
#if YYDEBUG

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab

namespace pgscript
{
#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  pgsParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

#endif

  /// Build a parser object.
  pgsParser::pgsParser (class pgsDriver & driver_yyarg)
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
      driver (driver_yyarg)
  {
  }

  pgsParser::~pgsParser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  pgsParser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  pgsParser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif /* ! YYDEBUG */

  void
  pgsParser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
        case 42: /* "\"ABORT\"" */
#line 167 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 217 "pgscript/parser.tab.cc"
	break;
      case 43: /* "\"ALTER\"" */
#line 168 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 222 "pgscript/parser.tab.cc"
	break;
      case 44: /* "\"ANALYZE\"" */
#line 169 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 227 "pgscript/parser.tab.cc"
	break;
      case 45: /* "\"BEGIN\"" */
#line 170 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 232 "pgscript/parser.tab.cc"
	break;
      case 46: /* "\"CHECKPOINT\"" */
#line 171 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 237 "pgscript/parser.tab.cc"
	break;
      case 47: /* "\"CLOSE\"" */
#line 172 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 242 "pgscript/parser.tab.cc"
	break;
      case 48: /* "\"CLUSTER\"" */
#line 173 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 247 "pgscript/parser.tab.cc"
	break;
      case 49: /* "\"COMMENT\"" */
#line 174 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 252 "pgscript/parser.tab.cc"
	break;
      case 50: /* "\"COMMIT\"" */
#line 175 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 257 "pgscript/parser.tab.cc"
	break;
      case 51: /* "\"COPY\"" */
#line 176 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 262 "pgscript/parser.tab.cc"
	break;
      case 52: /* "\"CREATE\"" */
#line 177 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 267 "pgscript/parser.tab.cc"
	break;
      case 53: /* "\"DEALLOCATE\"" */
#line 178 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 272 "pgscript/parser.tab.cc"
	break;
      case 54: /* "\"DECLARE\"" */
#line 179 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 277 "pgscript/parser.tab.cc"
	break;
      case 55: /* "\"DELETE\"" */
#line 180 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 282 "pgscript/parser.tab.cc"
	break;
      case 56: /* "\"DISCARD\"" */
#line 181 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 287 "pgscript/parser.tab.cc"
	break;
      case 57: /* "\"DROP\"" */
#line 182 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 292 "pgscript/parser.tab.cc"
	break;
      case 58: /* "\"END\"" */
#line 183 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 297 "pgscript/parser.tab.cc"
	break;
      case 59: /* "\"EXECUTE\"" */
#line 184 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 302 "pgscript/parser.tab.cc"
	break;
      case 60: /* "\"EXPLAIN\"" */
#line 185 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 307 "pgscript/parser.tab.cc"
	break;
      case 61: /* "\"FETCH\"" */
#line 186 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 312 "pgscript/parser.tab.cc"
	break;
      case 62: /* "\"GRANT\"" */
#line 187 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 317 "pgscript/parser.tab.cc"
	break;
      case 63: /* "\"INSERT\"" */
#line 188 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 322 "pgscript/parser.tab.cc"
	break;
      case 64: /* "\"LISTEN\"" */
#line 189 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 327 "pgscript/parser.tab.cc"
	break;
      case 65: /* "\"LOAD\"" */
#line 190 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 332 "pgscript/parser.tab.cc"
	break;
      case 66: /* "\"LOCK\"" */
#line 191 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 337 "pgscript/parser.tab.cc"
	break;
      case 67: /* "\"MOVE\"" */
#line 192 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 342 "pgscript/parser.tab.cc"
	break;
      case 68: /* "\"NOTIFY\"" */
#line 193 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 347 "pgscript/parser.tab.cc"
	break;
      case 69: /* "\"PREPARE\"" */
#line 194 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 352 "pgscript/parser.tab.cc"
	break;
      case 70: /* "\"REASSIGN\"" */
#line 195 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 357 "pgscript/parser.tab.cc"
	break;
      case 71: /* "\"REINDEX\"" */
#line 196 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 362 "pgscript/parser.tab.cc"
	break;
      case 72: /* "\"RELEASE\"" */
#line 197 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 367 "pgscript/parser.tab.cc"
	break;
      case 73: /* "\"RESET\"" */
#line 198 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 372 "pgscript/parser.tab.cc"
	break;
      case 74: /* "\"REVOKE\"" */
#line 199 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 377 "pgscript/parser.tab.cc"
	break;
      case 75: /* "\"ROLLBACK\"" */
#line 200 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 382 "pgscript/parser.tab.cc"
	break;
      case 76: /* "\"SAVEPOINT\"" */
#line 201 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 387 "pgscript/parser.tab.cc"
	break;
      case 77: /* "\"SELECT\"" */
#line 202 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 392 "pgscript/parser.tab.cc"
	break;
      case 78: /* "\"SET\"" */
#line 203 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 397 "pgscript/parser.tab.cc"
	break;
      case 79: /* "\"SHOW\"" */
#line 204 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 402 "pgscript/parser.tab.cc"
	break;
      case 80: /* "\"START\"" */
#line 205 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 407 "pgscript/parser.tab.cc"
	break;
      case 81: /* "\"TRUNCATE\"" */
#line 206 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 412 "pgscript/parser.tab.cc"
	break;
      case 82: /* "\"UNLISTEN\"" */
#line 207 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 417 "pgscript/parser.tab.cc"
	break;
      case 83: /* "\"UPDATE\"" */
#line 208 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 422 "pgscript/parser.tab.cc"
	break;
      case 84: /* "\"VACUUM\"" */
#line 209 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 427 "pgscript/parser.tab.cc"
	break;
      case 85: /* "\"VALUES\"" */
#line 210 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 432 "pgscript/parser.tab.cc"
	break;
      case 86: /* "\"IDENTIFIER\"" */
#line 212 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 437 "pgscript/parser.tab.cc"
	break;
      case 87: /* "\"INTEGER VALUE\"" */
#line 213 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 442 "pgscript/parser.tab.cc"
	break;
      case 88: /* "\"REAL VALUE\"" */
#line 214 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 447 "pgscript/parser.tab.cc"
	break;
      case 89: /* "\"STRING VALUE\"" */
#line 215 "pgscript/pgsParser.yy"
	{ pdelete((yyvaluep->str)); };
#line 452 "pgscript/parser.tab.cc"
	break;

	default:
	  break;
      }
  }

  void
  pgsParser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

  std::ostream&
  pgsParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  pgsParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  pgsParser::debug_level_type
  pgsParser::debug_level () const
  {
    return yydebug_;
  }

  void
  pgsParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }


  int
  pgsParser::parse ()
  {
    /// Look-ahead and look-ahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the look-ahead.
    semantic_type yylval;
    /// Location of the look-ahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* User initialization code.  */
    #line 43 "pgscript/pgsParser.yy"
{
	// Initialize the initial location object
	yylloc.begin.filename = yylloc.end.filename;
}
  /* Line 547 of yacc.c.  */
#line 534 "pgscript/parser.tab.cc"
    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;
    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without look-ahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a look-ahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Accept?  */
    if (yyn == yyfinal_)
      goto yyacceptlab;

    /* Shift the look-ahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted unless it is eof.  */
    if (yychar != yyeof_)
      yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:
#line 268 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsIdentRecord(*((yysemantic_stack_[(7) - (1)].str)), (yysemantic_stack_[(7) - (3)].expr), (yysemantic_stack_[(7) - (6)].expr));
									pdelete((yysemantic_stack_[(7) - (1)].str));
									driver.context.pop_var(); driver.context.pop_var(); // $3 & $6
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 3:
#line 275 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsIdentRecord(*((yysemantic_stack_[(4) - (1)].str)), (yysemantic_stack_[(4) - (3)].expr));
									pdelete((yysemantic_stack_[(4) - (1)].str));
									driver.context.pop_var(); // $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 4:
#line 282 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsLines(*((yysemantic_stack_[(4) - (3)].str)));
									pdelete((yysemantic_stack_[(4) - (3)].str));
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 5:
#line 288 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsColumns(*((yysemantic_stack_[(4) - (3)].str)));
									pdelete((yysemantic_stack_[(4) - (3)].str));
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 6:
#line 294 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsTrim((yysemantic_stack_[(4) - (3)].expr));
									driver.context.pop_var(); // $3
									driver.context.push_var((yyval.expr)); // assert
								;}
    break;

  case 7:
#line 300 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsIdent(*((yysemantic_stack_[(1) - (1)].str)));
									pdelete((yysemantic_stack_[(1) - (1)].str));
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 8:
#line 306 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsNumber(*((yysemantic_stack_[(1) - (1)].str)), pgsInt);
									pdelete((yysemantic_stack_[(1) - (1)].str));
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 9:
#line 312 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsNumber(*((yysemantic_stack_[(1) - (1)].str)), pgsReal);
									pdelete((yysemantic_stack_[(1) - (1)].str));
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 10:
#line 318 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsString(*((yysemantic_stack_[(1) - (1)].str)));
									pdelete((yysemantic_stack_[(1) - (1)].str));
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 11:
#line 323 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsExecute(*((yysemantic_stack_[(3) - (2)].str)), &driver.context.m_cout,
											&(driver.thread));
									pdelete((yysemantic_stack_[(3) - (2)].str));
									driver.context.push_var((yyval.expr)); // SQL Expression statement
								;}
    break;

  case 12:
#line 329 "pgscript/pgsParser.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); ;}
    break;

  case 13:
#line 330 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsParenthesis((yysemantic_stack_[(3) - (2)].expr));
									driver.context.pop_var(); // $2
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 14:
#line 338 "pgscript/pgsParser.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); ;}
    break;

  case 15:
#line 339 "pgscript/pgsParser.yy"
    { (yyval.expr) = (yysemantic_stack_[(2) - (2)].expr); ;}
    break;

  case 16:
#line 340 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsNegate((yysemantic_stack_[(2) - (2)].expr));
									driver.context.pop_var(); // $2
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 17:
#line 346 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsNot((yysemantic_stack_[(2) - (2)].expr));
									driver.context.pop_var(); // $2
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 18:
#line 354 "pgscript/pgsParser.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); ;}
    break;

  case 19:
#line 356 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsCast((yysemantic_stack_[(6) - (5)].integer), (yysemantic_stack_[(6) - (3)].expr));
									driver.context.pop_var(); // $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 20:
#line 364 "pgscript/pgsParser.yy"
    { (yyval.integer) = pgscript::pgsParser::token::PGS_INTEGER; ;}
    break;

  case 21:
#line 365 "pgscript/pgsParser.yy"
    { (yyval.integer) = pgscript::pgsParser::token::PGS_REAL; ;}
    break;

  case 22:
#line 366 "pgscript/pgsParser.yy"
    { (yyval.integer) = pgscript::pgsParser::token::PGS_STRING; ;}
    break;

  case 23:
#line 367 "pgscript/pgsParser.yy"
    { (yyval.integer) = pgscript::pgsParser::token::PGS_RECORD; ;}
    break;

  case 24:
#line 371 "pgscript/pgsParser.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); ;}
    break;

  case 25:
#line 373 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsTimes((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 26:
#line 380 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsOver((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 27:
#line 387 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsModulo((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 28:
#line 396 "pgscript/pgsParser.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); ;}
    break;

  case 29:
#line 398 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsPlus((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 30:
#line 405 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsMinus((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 31:
#line 414 "pgscript/pgsParser.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); ;}
    break;

  case 32:
#line 416 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsLower((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 33:
#line 423 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGreater((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 34:
#line 430 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsLowerEqual((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 35:
#line 437 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGreaterEqual((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 36:
#line 446 "pgscript/pgsParser.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); ;}
    break;

  case 37:
#line 448 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsEqual((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 38:
#line 455 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsEqual((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr), false);
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 39:
#line 462 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsDifferent((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 40:
#line 471 "pgscript/pgsParser.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); ;}
    break;

  case 41:
#line 473 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsAnd((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 42:
#line 482 "pgscript/pgsParser.yy"
    { (yyval.expr) = (yysemantic_stack_[(1) - (1)].expr); ;}
    break;

  case 43:
#line 484 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsOr((yysemantic_stack_[(3) - (1)].expr), (yysemantic_stack_[(3) - (3)].expr));
									driver.context.pop_var();
									driver.context.pop_var(); // $1 & $3
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 44:
#line 493 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("%s"), (yysemantic_stack_[(1) - (1)].expr)->value().c_str());
									(yyval.expr) = (yysemantic_stack_[(1) - (1)].expr);
								;}
    break;

  case 45:
#line 501 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenInt((yysemantic_stack_[(6) - (3)].expr), (yysemantic_stack_[(6) - (5)].expr), driver.context.zero(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 46:
#line 509 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenInt((yysemantic_stack_[(8) - (3)].expr), (yysemantic_stack_[(8) - (5)].expr), (yysemantic_stack_[(8) - (7)].expr), driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 47:
#line 516 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenInt((yysemantic_stack_[(10) - (3)].expr), (yysemantic_stack_[(10) - (5)].expr), (yysemantic_stack_[(10) - (7)].expr), (yysemantic_stack_[(10) - (9)].expr));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 48:
#line 523 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenReal((yysemantic_stack_[(8) - (3)].expr), (yysemantic_stack_[(8) - (5)].expr), (yysemantic_stack_[(8) - (7)].expr), driver.context.zero(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 49:
#line 532 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenReal((yysemantic_stack_[(10) - (3)].expr), (yysemantic_stack_[(10) - (5)].expr), (yysemantic_stack_[(10) - (7)].expr), (yysemantic_stack_[(10) - (9)].expr), driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 50:
#line 540 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenReal((yysemantic_stack_[(12) - (3)].expr), (yysemantic_stack_[(12) - (5)].expr), (yysemantic_stack_[(12) - (7)].expr), (yysemantic_stack_[(12) - (9)].expr), (yysemantic_stack_[(12) - (11)].expr));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 51:
#line 548 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenString((yysemantic_stack_[(6) - (3)].expr), (yysemantic_stack_[(6) - (5)].expr), driver.context.one(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 52:
#line 556 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenString((yysemantic_stack_[(8) - (3)].expr), (yysemantic_stack_[(8) - (5)].expr), (yysemantic_stack_[(8) - (7)].expr), driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 53:
#line 563 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenString((yysemantic_stack_[(10) - (3)].expr), (yysemantic_stack_[(10) - (5)].expr), (yysemantic_stack_[(10) - (7)].expr), (yysemantic_stack_[(10) - (9)].expr));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 54:
#line 570 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenRegex((yysemantic_stack_[(4) - (3)].expr), driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 55:
#line 576 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenRegex((yysemantic_stack_[(6) - (3)].expr), (yysemantic_stack_[(6) - (5)].expr));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 56:
#line 582 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenDictionary((yysemantic_stack_[(4) - (3)].expr), driver.context.zero(),
											driver.context.seed(), driver.context.encoding());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 57:
#line 590 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenDictionary((yysemantic_stack_[(6) - (3)].expr), (yysemantic_stack_[(6) - (5)].expr), driver.context.seed(),
											driver.context.encoding());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 58:
#line 598 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenDictionary((yysemantic_stack_[(8) - (3)].expr), (yysemantic_stack_[(8) - (5)].expr), (yysemantic_stack_[(8) - (7)].expr), driver.context.encoding());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 59:
#line 605 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenDictionary((yysemantic_stack_[(10) - (3)].expr), (yysemantic_stack_[(10) - (5)].expr), (yysemantic_stack_[(10) - (7)].expr), (yysemantic_stack_[(10) - (9)].expr));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 60:
#line 612 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenDate((yysemantic_stack_[(6) - (3)].expr), (yysemantic_stack_[(6) - (5)].expr), driver.context.zero(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 61:
#line 620 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenDate((yysemantic_stack_[(8) - (3)].expr), (yysemantic_stack_[(8) - (5)].expr), (yysemantic_stack_[(8) - (7)].expr), driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 62:
#line 627 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenDate((yysemantic_stack_[(10) - (3)].expr), (yysemantic_stack_[(10) - (5)].expr), (yysemantic_stack_[(10) - (7)].expr), (yysemantic_stack_[(10) - (9)].expr));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 63:
#line 634 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenTime((yysemantic_stack_[(6) - (3)].expr), (yysemantic_stack_[(6) - (5)].expr), driver.context.zero(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 64:
#line 642 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenTime((yysemantic_stack_[(8) - (3)].expr), (yysemantic_stack_[(8) - (5)].expr), (yysemantic_stack_[(8) - (7)].expr), driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 65:
#line 649 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenTime((yysemantic_stack_[(10) - (3)].expr), (yysemantic_stack_[(10) - (5)].expr), (yysemantic_stack_[(10) - (7)].expr), (yysemantic_stack_[(10) - (9)].expr));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 66:
#line 656 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenDateTime((yysemantic_stack_[(6) - (3)].expr), (yysemantic_stack_[(6) - (5)].expr), driver.context.zero(),
											driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 67:
#line 664 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenDateTime((yysemantic_stack_[(8) - (3)].expr), (yysemantic_stack_[(8) - (5)].expr), (yysemantic_stack_[(8) - (7)].expr), driver.context.seed());
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 68:
#line 671 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenDateTime((yysemantic_stack_[(10) - (3)].expr), (yysemantic_stack_[(10) - (5)].expr), (yysemantic_stack_[(10) - (7)].expr), (yysemantic_stack_[(10) - (9)].expr));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 69:
#line 678 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenReference((yysemantic_stack_[(6) - (3)].expr), (yysemantic_stack_[(6) - (5)].expr), driver.context.zero(),
											driver.context.seed(), &(driver.thread));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 70:
#line 686 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenReference((yysemantic_stack_[(8) - (3)].expr), (yysemantic_stack_[(8) - (5)].expr), (yysemantic_stack_[(8) - (7)].expr), driver.context.seed(),
											&(driver.thread));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 71:
#line 694 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsGenReference((yysemantic_stack_[(10) - (3)].expr), (yysemantic_stack_[(10) - (5)].expr), (yysemantic_stack_[(10) - (7)].expr), (yysemantic_stack_[(10) - (9)].expr), &(driver.thread));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.push_var((yyval.expr));
								;}
    break;

  case 72:
#line 703 "pgscript/pgsParser.yy"
    { (yyval.stmt) = (yysemantic_stack_[(1) - (1)].stmt); ;}
    break;

  case 73:
#line 704 "pgscript/pgsParser.yy"
    { (yyval.stmt) = (yysemantic_stack_[(1) - (1)].stmt); ;}
    break;

  case 74:
#line 705 "pgscript/pgsParser.yy"
    { (yyval.stmt) = (yysemantic_stack_[(1) - (1)].stmt); ;}
    break;

  case 75:
#line 706 "pgscript/pgsParser.yy"
    { (yyval.stmt) = (yysemantic_stack_[(2) - (1)].stmt); ;}
    break;

  case 76:
#line 707 "pgscript/pgsParser.yy"
    { (yyval.stmt) = (yysemantic_stack_[(2) - (1)].stmt); ;}
    break;

  case 77:
#line 708 "pgscript/pgsParser.yy"
    { (yyval.stmt) = (yysemantic_stack_[(2) - (1)].stmt); ;}
    break;

  case 78:
#line 709 "pgscript/pgsParser.yy"
    { (yyval.stmt) = (yysemantic_stack_[(2) - (1)].stmt); ;}
    break;

  case 79:
#line 710 "pgscript/pgsParser.yy"
    { (yyval.stmt) = (yysemantic_stack_[(2) - (1)].stmt); ;}
    break;

  case 80:
#line 714 "pgscript/pgsParser.yy"
    {
									driver.context.pop_stmt(); // $1
									(yyval.stmt_list) = driver.context.stmt_list(&(driver.thread));
									(yyval.stmt_list)->insert_back((yysemantic_stack_[(1) - (1)].stmt));
								;}
    break;

  case 81:
#line 719 "pgscript/pgsParser.yy"
    {
									driver.context.pop_stmt(); // $2
									(yyval.stmt_list) = (yysemantic_stack_[(2) - (1)].stmt_list);						
									(yyval.stmt_list)->insert_back((yysemantic_stack_[(2) - (2)].stmt));
								;}
    break;

  case 82:
#line 727 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("BEGIN END"));
									(yyval.stmt) = driver.context.stmt_list(&(driver.thread));
								;}
    break;

  case 83:
#line 732 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("BEGIN ... END"));
									(yyval.stmt) = (yysemantic_stack_[(3) - (2)].stmt_list);
								;}
    break;

  case 84:
#line 739 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("%s"), (yysemantic_stack_[(1) - (1)].expr)->value().c_str());
									(yyval.stmt) = pnew pgsExpressionStmt((yysemantic_stack_[(1) - (1)].expr), &(driver.thread));
									driver.context.pop_var(); // $1
									driver.context.push_stmt((yyval.stmt)); // pgsExpressionStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
								;}
    break;

  case 85:
#line 749 "pgscript/pgsParser.yy"
    {
									(yyval.expr) = pnew pgsExecute(*((yysemantic_stack_[(1) - (1)].str)), &driver.context.m_cout,
											&(driver.thread));
									pdelete((yysemantic_stack_[(1) - (1)].str));
									driver.context.push_var((yyval.expr)); // pgsExecute
								;}
    break;

  case 86:
#line 758 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 87:
#line 759 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 88:
#line 760 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 89:
#line 761 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 90:
#line 762 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 91:
#line 763 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 92:
#line 764 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 93:
#line 765 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 94:
#line 766 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 95:
#line 767 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 96:
#line 768 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 97:
#line 769 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 98:
#line 770 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 99:
#line 771 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 100:
#line 772 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 101:
#line 773 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 102:
#line 774 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 103:
#line 775 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 104:
#line 776 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 105:
#line 777 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 106:
#line 778 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 107:
#line 779 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 108:
#line 780 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 109:
#line 781 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 110:
#line 782 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 111:
#line 783 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 112:
#line 784 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 113:
#line 785 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 114:
#line 786 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 115:
#line 787 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 116:
#line 788 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 117:
#line 789 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 118:
#line 790 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 119:
#line 791 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 120:
#line 792 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 121:
#line 793 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 122:
#line 794 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 123:
#line 795 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 124:
#line 796 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 125:
#line 797 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 126:
#line 798 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 127:
#line 799 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 128:
#line 800 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 129:
#line 801 "pgscript/pgsParser.yy"
    { (yyval.str) = (yysemantic_stack_[(1) - (1)].str); ;}
    break;

  case 130:
#line 806 "pgscript/pgsParser.yy"
    {
									(yyval.stmt) = (yysemantic_stack_[(2) - (2)].stmt_list);
								;}
    break;

  case 131:
#line 812 "pgscript/pgsParser.yy"
    {
									driver.context.pop_stmt(); // $1
									(yyval.stmt_list) = driver.context.stmt_list(&(driver.thread));
									(yyval.stmt_list)->insert_back((yysemantic_stack_[(1) - (1)].stmt));

								;}
    break;

  case 132:
#line 819 "pgscript/pgsParser.yy"
    {
									driver.context.pop_stmt(); // $3
									(yyval.stmt_list) = (yysemantic_stack_[(3) - (1)].stmt_list);
									(yyval.stmt_list)->insert_back((yysemantic_stack_[(3) - (3)].stmt));
								;}
    break;

  case 133:
#line 827 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("DECLARE %s"), (yysemantic_stack_[(1) - (1)].str)->c_str());
									
									(yyval.stmt) = pnew pgsExpressionStmt(pnew pgsAssign(*((yysemantic_stack_[(1) - (1)].str)),
											pnew pgsString(wxT(""))), &(driver.thread));
									driver.context.push_stmt((yyval.stmt)); // pgsExpressionStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
									
									pdelete((yysemantic_stack_[(1) - (1)].str));
								;}
    break;

  case 134:
#line 838 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("DECLARE %s"), (yysemantic_stack_[(4) - (1)].str)->c_str());
									
									(yyval.stmt) = pnew pgsDeclareRecordStmt(*((yysemantic_stack_[(4) - (1)].str)), driver.context.columns(),
											&(driver.thread));
									driver.context.push_stmt((yyval.stmt)); // pgsDeclareRecordStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
									
									driver.context.clear_columns();
									pdelete((yysemantic_stack_[(4) - (1)].str));
								;}
    break;

  case 135:
#line 853 "pgscript/pgsParser.yy"
    {
									(yyval.stmt) = (yysemantic_stack_[(2) - (2)].stmt_list);
								;}
    break;

  case 136:
#line 859 "pgscript/pgsParser.yy"
    {
									driver.context.pop_stmt(); // $1
									(yyval.stmt_list) = driver.context.stmt_list(&(driver.thread));
									(yyval.stmt_list)->insert_back((yysemantic_stack_[(1) - (1)].stmt));
								;}
    break;

  case 137:
#line 865 "pgscript/pgsParser.yy"
    {
									driver.context.pop_stmt(); // $3
									(yyval.stmt_list) = (yysemantic_stack_[(3) - (1)].stmt_list);
									(yyval.stmt_list)->insert_back((yysemantic_stack_[(3) - (3)].stmt));
								;}
    break;

  case 138:
#line 874 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("SET %s = %s"), (yysemantic_stack_[(3) - (1)].str)->c_str(),
											(yysemantic_stack_[(3) - (3)].expr)->value().c_str());
									
									(yyval.stmt) = pnew pgsExpressionStmt(pnew pgsAssign(*((yysemantic_stack_[(3) - (1)].str)), (yysemantic_stack_[(3) - (3)].expr)),
											&(driver.thread));
									driver.context.pop_var(); // $3
									driver.context.push_stmt((yyval.stmt)); // pgsExpressionStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
									
									pdelete((yysemantic_stack_[(3) - (1)].str));
								;}
    break;

  case 139:
#line 887 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("SET %s[%s][%s] = %s"),
											(yysemantic_stack_[(9) - (1)].str)->c_str(), (yysemantic_stack_[(9) - (3)].expr)->value().c_str(),
											(yysemantic_stack_[(9) - (6)].expr)->value().c_str(), (yysemantic_stack_[(9) - (9)].expr)->value().c_str());
									
									(yyval.stmt) = pnew pgsExpressionStmt(pnew pgsAssignToRecord(*((yysemantic_stack_[(9) - (1)].str)),
											(yysemantic_stack_[(9) - (3)].expr), (yysemantic_stack_[(9) - (6)].expr), (yysemantic_stack_[(9) - (9)].expr)), &(driver.thread));
									driver.context.pop_var(); driver.context.pop_var();
									driver.context.pop_var(); // $3 & $6 & $9
									driver.context.push_stmt((yyval.stmt)); // pgsExpressionStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
									
									pdelete((yysemantic_stack_[(9) - (1)].str));
								;}
    break;

  case 140:
#line 902 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("SET %s = %s"), (yysemantic_stack_[(3) - (1)].str)->c_str(),
											(yysemantic_stack_[(3) - (3)].expr)->value().c_str());
									
									(yyval.stmt) = pnew pgsExpressionStmt(pnew pgsAssign(*((yysemantic_stack_[(3) - (1)].str)), (yysemantic_stack_[(3) - (3)].expr)),
											&(driver.thread));
									driver.context.pop_var(); // $3
									driver.context.push_stmt((yyval.stmt)); // pgsExpressionStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
									
									pdelete((yysemantic_stack_[(3) - (1)].str));
								;}
    break;

  case 141:
#line 918 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("IF %s"), (yysemantic_stack_[(3) - (2)].expr)->value().c_str());
									
									(yyval.stmt) = pnew pgsIfStmt((yysemantic_stack_[(3) - (2)].expr), (yysemantic_stack_[(3) - (3)].stmt), driver.context
											.stmt_list(&(driver.thread)), &(driver.thread));
									driver.context.pop_var(); // $2
									driver.context.pop_stmt(); // $3
									driver.context.pop_stmt(); // stmt_list
									driver.context.push_stmt((yyval.stmt)); // pgsIfStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
								;}
    break;

  case 142:
#line 930 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("IF %s"), (yysemantic_stack_[(5) - (2)].expr)->value().c_str());
									
									(yyval.stmt) = pnew pgsIfStmt((yysemantic_stack_[(5) - (2)].expr), (yysemantic_stack_[(5) - (3)].stmt), (yysemantic_stack_[(5) - (5)].stmt), &(driver.thread));
									driver.context.pop_var(); // $2
									driver.context.pop_stmt(); // $3
									driver.context.pop_stmt(); // $5
									driver.context.push_stmt((yyval.stmt)); // pgsIfStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
								;}
    break;

  case 143:
#line 944 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("WHILE %s"), (yysemantic_stack_[(3) - (2)].expr)->value().c_str());
									
									(yyval.stmt) = pnew pgsWhileStmt((yysemantic_stack_[(3) - (2)].expr), (yysemantic_stack_[(3) - (3)].stmt), &(driver.thread));
									driver.context.pop_var(); // $2
									driver.context.pop_stmt(); // $3
									driver.context.push_stmt((yyval.stmt)); // pgsWhileStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
								;}
    break;

  case 144:
#line 956 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("BREAK"));
									
									(yyval.stmt) = pnew pgsBreakStmt(&(driver.thread));
									driver.context.push_stmt((yyval.stmt)); // pgsBreakStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
								;}
    break;

  case 145:
#line 963 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("RETURN"));
									
									(yyval.stmt) = pnew pgsBreakStmt(&(driver.thread));
									driver.context.push_stmt((yyval.stmt)); // pgsBreakStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
								;}
    break;

  case 146:
#line 970 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("CONTINUE"));
									
									(yyval.stmt) = pnew pgsContinueStmt(&(driver.thread));
									driver.context.push_stmt((yyval.stmt)); // pgsContinueStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
								;}
    break;

  case 147:
#line 981 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("PRINT %s"), (yysemantic_stack_[(2) - (2)].expr)->value().c_str());
									
									(yyval.stmt) = pnew pgsPrintStmt((yysemantic_stack_[(2) - (2)].expr), driver.context.m_cout,
											&(driver.thread));
									driver.context.pop_var(); // $2
									driver.context.push_stmt((yyval.stmt)); // pgsPrintStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
								;}
    break;

  case 148:
#line 991 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("ASSERT %s"), (yysemantic_stack_[(2) - (2)].expr)->value().c_str());
									
									(yyval.stmt) = pnew pgsAssertStmt((yysemantic_stack_[(2) - (2)].expr), &(driver.thread));
									driver.context.pop_var(); // $2
									driver.context.push_stmt((yyval.stmt)); // pgsAssertStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
								;}
    break;

  case 149:
#line 1000 "pgscript/pgsParser.yy"
    {
									wxLogScriptVerbose(wxT("RMLINE %s[%s]"), (yysemantic_stack_[(7) - (3)].str)->c_str(),
											(yysemantic_stack_[(7) - (5)].expr)->value().c_str());
									
									(yyval.stmt) = pnew pgsExpressionStmt(pnew pgsRemoveLine(*((yysemantic_stack_[(7) - (3)].str)), (yysemantic_stack_[(7) - (5)].expr)),
											&(driver.thread));
									driver.context.pop_var(); // $5
									driver.context.push_stmt((yyval.stmt)); // pgsExpressionStmt
									(yyval.stmt)->set_position(yyloc.begin.line);
									
									pdelete((yysemantic_stack_[(7) - (3)].str));
								;}
    break;

  case 150:
#line 1015 "pgscript/pgsParser.yy"
    {
									driver.context.add_column(*(yysemantic_stack_[(1) - (1)].str));
									pdelete((yysemantic_stack_[(1) - (1)].str));
								;}
    break;

  case 151:
#line 1020 "pgscript/pgsParser.yy"
    {
									driver.context.add_column(*(yysemantic_stack_[(3) - (3)].str));
									pdelete((yysemantic_stack_[(3) - (3)].str));
								;}
    break;

  case 153:
#line 1028 "pgscript/pgsParser.yy"
    {
									driver.program.eval((yysemantic_stack_[(2) - (1)].stmt_list));
									
									driver.context.pop_stmt();
									pdelete((yysemantic_stack_[(2) - (1)].stmt_list)); // delete root statement $1
								;}
    break;


    /* Line 675 of lalr1.cc.  */
#line 1866 "pgscript/parser.tab.cc"
	default: break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse look-ahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    if (yyn == yyfinal_)
      goto yyacceptlab;

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the look-ahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		   &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyeof_ && yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  pgsParser::yysyntax_error_ (int yystate, int tok)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char pgsParser::yypact_ninf_ = -93;
  const short int
  pgsParser::yypact_[] =
  {
       284,   -93,     6,   -93,   -93,   -93,     6,   453,     6,     6,
     -88,   -76,   -61,   -93,   -93,   -93,   -93,   -93,   -93,   -93,
     -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,
     -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,
     -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,
     -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   370,   -93,
     -65,   -93,   -93,   -56,   -45,   -93,   -93,   -44,   -28,    73,
      12,    13,    23,    27,    37,    42,    44,    50,    63,    65,
      66,    67,    68,     6,     0,   -93,   -93,   -93,   -11,     6,
       6,   -93,   -93,   -93,    51,   -74,     7,   117,   120,   124,
     619,   -93,   619,   -93,   536,   -93,   -93,    76,   -25,    62,
     -93,    61,    69,   -93,   -93,   -93,   -93,   -93,   -93,   -93,
     -93,   -93,    79,    80,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   -93,     6,    74,    75,   -93,
     -93,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   -93,   161,   -93,    81,   689,
       6,   -76,    86,   -61,    82,    83,    84,   163,    77,    78,
      85,   -53,   -52,    87,    89,    90,    91,    92,   -93,   -93,
     -93,   -93,   -93,    51,    51,   -74,   -74,   -74,   -74,     7,
       7,     7,   117,   120,   619,     6,   -93,   -93,    93,   -93,
     -93,    34,   -93,   -93,   -93,   -93,    64,     6,     6,     6,
     -93,     6,   -93,     6,     6,     6,     6,     6,    95,   -93,
      96,   103,    88,   -93,   -93,   -93,   -93,   -93,   101,   -51,
      94,   -42,   104,   -38,   -37,   -31,   -22,   -21,     6,   105,
       6,   -93,   -93,   -93,     6,     6,   -93,     6,   -93,   -93,
       6,   -93,     6,   -93,     6,   -93,     6,   -93,     6,   108,
     -93,   109,    -4,    -2,     9,    10,    15,    16,    38,    39,
     -93,   147,   -93,     6,   -93,     6,   -93,     6,   -93,     6,
     -93,     6,   -93,     6,   -93,     6,   -93,     6,     6,   111,
      40,   113,   119,   121,   122,   123,   125,   -93,   -93,   -93,
       6,   -93,   -93,   -93,   -93,   -93,   -93,   126,   -93
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  pgsParser::yydefact_[] =
  {
         0,   152,     0,   144,   145,   146,     0,     0,     0,     0,
       0,     0,     0,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,    80,     0,    72,
       0,    84,    85,     0,     0,    73,    74,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     7,     8,     9,    10,     0,     0,
       0,    14,    18,    24,    28,    31,    36,    40,    42,    44,
       0,    12,     0,    82,     0,   148,   147,     0,     0,   135,
     136,   133,   130,   131,   153,    81,    75,    78,    79,    77,
      76,     1,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    17,     0,     0,     0,    15,
      16,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   143,   141,    83,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    11,    13,
      25,    26,    27,    29,    30,    34,    35,    32,    33,    37,
      38,    39,    41,    43,     0,     0,   138,   140,     0,   137,
     150,     0,   132,     5,     4,     6,     0,     0,     0,     0,
      54,     0,    56,     0,     0,     0,     0,     0,     3,   142,
       0,     0,     0,   134,    23,    20,    21,    22,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   151,    19,    45,     0,     0,    51,     0,    55,    57,
       0,    60,     0,    63,     0,    66,     0,    69,     0,     0,
     149,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       2,     0,    46,     0,    48,     0,    52,     0,    58,     0,
      61,     0,    64,     0,    67,     0,    70,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   139,    47,    49,
       0,    53,    59,    62,    65,    68,    71,     0,    50
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  pgsParser::yypgoto_[] =
  {
       -93,   -93,   -93,   -29,   -93,   -92,    -3,   -83,    28,    59,
     -93,    -6,   -93,   -57,   175,   -93,   -93,    58,   -93,   -93,
     -93,    33,   -93,   -93,    60,   -93,   -93,   -93,   -93,   -93,
     -93
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  pgsParser::yydefgoto_[] =
  {
        -1,    91,    92,    93,   228,    94,    95,    96,    97,    98,
      99,   100,   101,    57,    58,    59,    60,    61,    62,    63,
     112,   113,    64,   109,   110,    65,    66,    67,    68,   201,
      69
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char pgsParser::yytable_ninf_ = -1;
  const unsigned short int
  pgsParser::yytable_[] =
  {
       102,   115,   105,   106,   107,    70,    71,    72,   159,    73,
     108,    74,    75,    76,    77,    78,    79,    80,    81,    82,
     144,   145,    70,    71,    72,   111,    73,    83,    74,    75,
      76,    77,    78,    79,    80,    81,    82,   116,   146,   147,
     210,   212,   243,   155,    83,   156,   117,   115,   211,   213,
     244,   246,   183,   184,   135,   249,   251,   118,   119,   247,
     139,   140,   253,   250,   252,   160,   137,   189,   190,   191,
     254,   255,   257,   121,   120,    84,    85,    86,    87,   256,
     258,    88,   138,    89,    90,   224,   225,   226,   227,   272,
     136,   274,    84,    85,    86,    87,   167,   273,    88,   275,
      89,    90,   276,   278,   122,   123,   148,   149,   280,   282,
     277,   279,   180,   181,   182,   124,   281,   283,   166,   125,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   126,
     177,   284,   286,   299,   127,   222,   128,   219,   223,   285,
     287,   300,   129,   185,   186,   187,   188,   141,   142,   143,
     150,   151,   152,   196,   198,   130,   153,   131,   132,   133,
     134,   154,   158,   161,   162,   164,   165,   178,   179,   194,
     163,   195,   200,   206,   241,   203,   204,   205,   207,   208,
     288,   192,   104,   218,   221,   238,   209,   239,   214,   220,
     215,   216,   217,   240,   242,   245,   202,   248,   260,   270,
     271,   229,   230,   231,   298,   232,   301,   233,   234,   235,
     236,   237,   302,   193,   303,   304,   305,   197,   306,   308,
       0,   199,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   259,     0,   261,     0,     0,     0,   262,   263,
       0,   264,     0,     0,   265,     0,   266,     0,   267,     0,
     268,     0,   269,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   289,     0,   290,
       0,   291,     0,   292,     0,   293,     0,   294,     0,   295,
       0,   296,   297,     0,     1,     0,     0,     2,     3,     4,
       5,     6,     0,     0,   307,     7,     0,     8,     9,     0,
       0,     0,     0,    10,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
     114,     0,     0,     2,     3,     4,     5,     6,     0,     0,
       0,     7,     0,     8,     9,     0,     0,     0,     0,    10,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,     2,     3,     4,     5,
       6,     0,     0,     0,     7,   103,     8,     9,     0,     0,
       0,     0,    10,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,     2,
       3,     4,     5,     6,     0,     0,     0,     7,   157,     8,
       9,     0,     0,     0,     0,    10,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,     2,     3,     4,     5,     6,     0,     0,     0,
       7,     0,     8,     9,     0,     0,     0,     0,    10,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    70,    71,    72,     0,    73,
       0,    74,    75,    76,    77,    78,    79,    80,    81,    82,
       0,     0,     0,     0,     0,     0,     0,    83,     0,     0,
       0,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    84,    85,    86,    87,     0,
       0,    88,     0,    89,    90
  };

  /* YYCHECK.  */
  const short int
  pgsParser::yycheck_[] =
  {
         6,    58,     8,     9,    92,    16,    17,    18,    33,    20,
      86,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      94,    95,    16,    17,    18,    86,    20,    38,    22,    23,
      24,    25,    26,    27,    28,    29,    30,   102,    31,    32,
      93,    93,    93,   100,    38,   102,   102,   104,   101,   101,
     101,    93,   144,   145,    83,    93,    93,   102,   102,   101,
      89,    90,    93,   101,   101,    90,    77,   150,   151,   152,
     101,    93,    93,     0,   102,    86,    87,    88,    89,   101,
     101,    92,    88,    94,    95,    21,    22,    23,    24,    93,
      90,    93,    86,    87,    88,    89,   125,   101,    92,   101,
      94,    95,    93,    93,    92,    92,    99,   100,    93,    93,
     101,   101,   141,   142,   143,    92,   101,   101,   124,    92,
     126,   127,   128,   129,   130,   131,   132,   133,   134,    92,
     136,    93,    93,    93,    92,   101,    92,   194,   104,   101,
     101,   101,    92,   146,   147,   148,   149,    96,    97,    98,
      33,    34,    35,   159,   160,    92,    36,    92,    92,    92,
      92,    37,    86,   101,   103,    86,    86,    93,    93,     8,
     101,    90,    86,    10,    86,    93,    93,    93,   101,   101,
      33,   153,     7,    91,    91,    90,   101,    91,   101,   195,
     101,   101,   101,    90,    93,   101,   163,    93,    93,    91,
      91,   207,   208,   209,    93,   211,    93,   213,   214,   215,
     216,   217,    93,   154,    93,    93,    93,   159,    93,    93,
      -1,   161,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   238,    -1,   240,    -1,    -1,    -1,   244,   245,
      -1,   247,    -1,    -1,   250,    -1,   252,    -1,   254,    -1,
     256,    -1,   258,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   273,    -1,   275,
      -1,   277,    -1,   279,    -1,   281,    -1,   283,    -1,   285,
      -1,   287,   288,    -1,     0,    -1,    -1,     3,     4,     5,
       6,     7,    -1,    -1,   300,    11,    -1,    13,    14,    -1,
      -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
       0,    -1,    -1,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    11,    -1,    13,    14,    -1,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,     3,
       4,     5,     6,     7,    -1,    -1,    -1,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    -1,    13,    14,    -1,    -1,    -1,    -1,    19,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    16,    17,    18,    -1,    20,
      -1,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    -1,
      -1,    92,    -1,    94,    95
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  pgsParser::yystos_[] =
  {
         0,     0,     3,     4,     5,     6,     7,    11,    13,    14,
      19,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,   118,   119,   120,
     121,   122,   123,   124,   127,   130,   131,   132,   133,   135,
      16,    17,    18,    20,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    38,    86,    87,    88,    89,    92,    94,
      95,   106,   107,   108,   110,   111,   112,   113,   114,   115,
     116,   117,   116,    12,   119,   116,   116,    92,    86,   128,
     129,    86,   125,   126,     0,   118,   102,   102,   102,   102,
     102,     0,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,   108,    90,    77,   116,   108,
     108,    96,    97,    98,    94,    95,    31,    32,    99,   100,
      33,    34,    35,    36,    37,   118,   118,    12,    86,    33,
      90,   101,   103,   101,    86,    86,   116,   108,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,    93,    93,
     108,   108,   108,   110,   110,   111,   111,   111,   111,   112,
     112,   112,   113,   114,     8,    90,   116,   122,   116,   129,
      86,   134,   126,    93,    93,    93,    10,   101,   101,   101,
      93,   101,    93,   101,   101,   101,   101,   101,    91,   118,
     116,    91,   101,   104,    21,    22,    23,    24,   109,   116,
     116,   116,   116,   116,   116,   116,   116,   116,    90,    91,
      90,    86,    93,    93,   101,   101,    93,   101,    93,    93,
     101,    93,   101,    93,   101,    93,   101,    93,   101,   116,
      93,   116,   116,   116,   116,   116,   116,   116,   116,   116,
      91,    91,    93,   101,    93,   101,    93,   101,    93,   101,
      93,   101,    93,   101,    93,   101,    93,   101,    33,   116,
     116,   116,   116,   116,   116,   116,   116,   116,    93,    93,
     101,    93,    93,    93,    93,    93,    93,   116,    93
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  pgsParser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
      91,    93,    40,    41,    43,    45,    42,    47,    37,    60,
      62,    44,    59,   123,   125
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  pgsParser::yyr1_[] =
  {
         0,   105,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   107,   107,   107,   107,   108,   108,
     109,   109,   109,   109,   110,   110,   110,   110,   111,   111,
     111,   112,   112,   112,   112,   112,   113,   113,   113,   113,
     114,   114,   115,   115,   116,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   118,   118,   118,   118,   118,   118,   118,   118,
     119,   119,   120,   120,   121,   122,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     124,   125,   125,   126,   126,   127,   128,   128,   129,   129,
     129,   130,   130,   131,   132,   132,   132,   133,   133,   133,
     134,   134,   135,   135
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  pgsParser::yyr2_[] =
  {
         0,     2,     7,     4,     4,     4,     4,     1,     1,     1,
       1,     3,     1,     3,     1,     2,     2,     2,     1,     6,
       1,     1,     1,     1,     1,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     3,
       1,     3,     1,     3,     1,     6,     8,    10,     8,    10,
      12,     6,     8,    10,     4,     6,     4,     6,     8,    10,
       6,     8,    10,     6,     8,    10,     6,     8,    10,     6,
       8,    10,     1,     1,     1,     2,     2,     2,     2,     2,
       1,     2,     2,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     3,     1,     4,     2,     1,     3,     3,     9,
       3,     3,     5,     3,     1,     1,     1,     2,     2,     7,
       1,     3,     1,     2
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const pgsParser::yytname_[] =
  {
    "\"END OF FILE\"", "error", "$undefined", "\"WHILE\"", "\"BREAK\"",
  "\"RETURN\"", "\"CONTINUE\"", "\"IF\"", "\"ELSE\"", "\"WAITFOR\"",
  "\"AS\"", "\"BEGIN (BLOCK)\"", "\"END (BLOCK)\"", "\"ASSERT\"",
  "\"PRINT\"", "\"LOG\"", "\"COLUMNS\"", "\"LINES\"", "\"TRIM\"",
  "\"RMLINE\"", "\"CAST\"", "\"RECORD\"", "\"INTEGER\"", "\"REAL\"",
  "\"STRING\"", "\"REGEX\"", "\"FILE\"", "\"DATE\"", "\"TIME\"",
  "\"DATETIME\"", "\"REFERENCE\"", "\"<=\"", "\">=\"", "\"=\"", "\"~=\"",
  "\"<>\"", "\"AND\"", "\"OR\"", "\"NOT\"", "\"character\"",
  "\"SET @VARIABLE\"", "\"DECLARE @VARIABLE\"", "\"ABORT\"", "\"ALTER\"",
  "\"ANALYZE\"", "\"BEGIN\"", "\"CHECKPOINT\"", "\"CLOSE\"", "\"CLUSTER\"",
  "\"COMMENT\"", "\"COMMIT\"", "\"COPY\"", "\"CREATE\"", "\"DEALLOCATE\"",
  "\"DECLARE\"", "\"DELETE\"", "\"DISCARD\"", "\"DROP\"", "\"END\"",
  "\"EXECUTE\"", "\"EXPLAIN\"", "\"FETCH\"", "\"GRANT\"", "\"INSERT\"",
  "\"LISTEN\"", "\"LOAD\"", "\"LOCK\"", "\"MOVE\"", "\"NOTIFY\"",
  "\"PREPARE\"", "\"REASSIGN\"", "\"REINDEX\"", "\"RELEASE\"", "\"RESET\"",
  "\"REVOKE\"", "\"ROLLBACK\"", "\"SAVEPOINT\"", "\"SELECT\"", "\"SET\"",
  "\"SHOW\"", "\"START\"", "\"TRUNCATE\"", "\"UNLISTEN\"", "\"UPDATE\"",
  "\"VACUUM\"", "\"VALUES\"", "\"IDENTIFIER\"", "\"INTEGER VALUE\"",
  "\"REAL VALUE\"", "\"STRING VALUE\"", "'['", "']'", "'('", "')'", "'+'",
  "'-'", "'*'", "'/'", "'%'", "'<'", "'>'", "','", "';'", "'{'", "'}'",
  "$accept", "postfix_expression", "unary_expression", "cast_expression",
  "type_name", "multiplicative_expression", "additive_expression",
  "relational_expression", "equality_expression", "logical_and_expression",
  "logical_or_expression", "expression", "random_generator", "statement",
  "statement_list", "compound_statement", "sql_statement",
  "sql_expression", "sql_query", "declaration_statement",
  "declaration_list", "declaration_element", "assign_statement",
  "assign_list", "assign_element", "selection_statement",
  "iteration_statement", "jump_statement", "procedure_statement",
  "record_declaration_list", "translation_unit", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const pgsParser::rhs_number_type
  pgsParser::yyrhs_[] =
  {
       135,     0,    -1,    86,    90,   116,    91,    90,   116,    91,
      -1,    86,    90,   116,    91,    -1,    17,    92,    86,    93,
      -1,    16,    92,    86,    93,    -1,    18,    92,   116,    93,
      -1,    86,    -1,    87,    -1,    88,    -1,    89,    -1,    92,
      77,    93,    -1,   117,    -1,    92,   116,    93,    -1,   106,
      -1,    94,   108,    -1,    95,   108,    -1,    38,   108,    -1,
     107,    -1,    20,    92,   108,    10,   109,    93,    -1,    22,
      -1,    23,    -1,    24,    -1,    21,    -1,   108,    -1,   110,
      96,   108,    -1,   110,    97,   108,    -1,   110,    98,   108,
      -1,   110,    -1,   111,    94,   110,    -1,   111,    95,   110,
      -1,   111,    -1,   112,    99,   111,    -1,   112,   100,   111,
      -1,   112,    31,   111,    -1,   112,    32,   111,    -1,   112,
      -1,   113,    33,   112,    -1,   113,    34,   112,    -1,   113,
      35,   112,    -1,   113,    -1,   114,    36,   113,    -1,   114,
      -1,   115,    37,   114,    -1,   115,    -1,    22,    92,   116,
     101,   116,    93,    -1,    22,    92,   116,   101,   116,   101,
     116,    93,    -1,    22,    92,   116,   101,   116,   101,   116,
     101,   116,    93,    -1,    23,    92,   116,   101,   116,   101,
     116,    93,    -1,    23,    92,   116,   101,   116,   101,   116,
     101,   116,    93,    -1,    23,    92,   116,   101,   116,   101,
     116,   101,   116,   101,   116,    93,    -1,    24,    92,   116,
     101,   116,    93,    -1,    24,    92,   116,   101,   116,   101,
     116,    93,    -1,    24,    92,   116,   101,   116,   101,   116,
     101,   116,    93,    -1,    25,    92,   116,    93,    -1,    25,
      92,   116,   101,   116,    93,    -1,    26,    92,   116,    93,
      -1,    26,    92,   116,   101,   116,    93,    -1,    26,    92,
     116,   101,   116,   101,   116,    93,    -1,    26,    92,   116,
     101,   116,   101,   116,   101,   116,    93,    -1,    27,    92,
     116,   101,   116,    93,    -1,    27,    92,   116,   101,   116,
     101,   116,    93,    -1,    27,    92,   116,   101,   116,   101,
     116,   101,   116,    93,    -1,    28,    92,   116,   101,   116,
      93,    -1,    28,    92,   116,   101,   116,   101,   116,    93,
      -1,    28,    92,   116,   101,   116,   101,   116,   101,   116,
      93,    -1,    29,    92,   116,   101,   116,    93,    -1,    29,
      92,   116,   101,   116,   101,   116,    93,    -1,    29,    92,
     116,   101,   116,   101,   116,   101,   116,    93,    -1,    30,
      92,   116,   101,   116,    93,    -1,    30,    92,   116,   101,
     116,   101,   116,    93,    -1,    30,    92,   116,   101,   116,
     101,   116,   101,   116,    93,    -1,   120,    -1,   130,    -1,
     131,    -1,   121,   102,    -1,   133,   102,    -1,   132,   102,
      -1,   124,   102,    -1,   127,   102,    -1,   118,    -1,   119,
     118,    -1,    11,    12,    -1,    11,   119,    12,    -1,   122,
      -1,   123,    -1,    42,    -1,    43,    -1,    44,    -1,    45,
      -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,
      -1,    51,    -1,    52,    -1,    53,    -1,    54,    -1,    55,
      -1,    56,    -1,    57,    -1,    58,    -1,    59,    -1,    60,
      -1,    61,    -1,    62,    -1,    63,    -1,    64,    -1,    65,
      -1,    66,    -1,    67,    -1,    68,    -1,    69,    -1,    70,
      -1,    71,    -1,    72,    -1,    73,    -1,    74,    -1,    75,
      -1,    76,    -1,    77,    -1,    78,    -1,    79,    -1,    80,
      -1,    81,    -1,    82,    -1,    83,    -1,    84,    -1,    85,
      -1,    41,   125,    -1,   126,    -1,   125,   101,   126,    -1,
      86,    -1,    86,   103,   134,   104,    -1,    40,   128,    -1,
     129,    -1,   128,   101,   129,    -1,    86,    33,   116,    -1,
      86,    90,   116,    91,    90,   116,    91,    33,   116,    -1,
      86,    33,   122,    -1,     7,   116,   118,    -1,     7,   116,
     118,     8,   118,    -1,     3,   116,   118,    -1,     4,    -1,
       5,    -1,     6,    -1,    14,   116,    -1,    13,   116,    -1,
      19,    92,    86,    90,   116,    91,    93,    -1,    86,    -1,
     134,   101,    86,    -1,     0,    -1,   119,     0,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  pgsParser::yyprhs_[] =
  {
         0,     0,     3,    11,    16,    21,    26,    31,    33,    35,
      37,    39,    43,    45,    49,    51,    54,    57,    60,    62,
      69,    71,    73,    75,    77,    79,    83,    87,    91,    93,
      97,   101,   103,   107,   111,   115,   119,   121,   125,   129,
     133,   135,   139,   141,   145,   147,   154,   163,   174,   183,
     194,   207,   214,   223,   234,   239,   246,   251,   258,   267,
     278,   285,   294,   305,   312,   321,   332,   339,   348,   359,
     366,   375,   386,   388,   390,   392,   395,   398,   401,   404,
     407,   409,   412,   415,   419,   421,   423,   425,   427,   429,
     431,   433,   435,   437,   439,   441,   443,   445,   447,   449,
     451,   453,   455,   457,   459,   461,   463,   465,   467,   469,
     471,   473,   475,   477,   479,   481,   483,   485,   487,   489,
     491,   493,   495,   497,   499,   501,   503,   505,   507,   509,
     511,   514,   516,   520,   522,   527,   530,   532,   536,   540,
     550,   554,   558,   564,   568,   570,   572,   574,   577,   580,
     588,   590,   594,   596
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  pgsParser::yyrline_[] =
  {
         0,   267,   267,   274,   281,   287,   293,   299,   305,   311,
     317,   323,   329,   330,   338,   339,   340,   345,   354,   355,
     364,   365,   366,   367,   371,   372,   379,   386,   396,   397,
     404,   414,   415,   422,   429,   436,   446,   447,   454,   461,
     471,   472,   482,   483,   493,   500,   508,   515,   522,   531,
     539,   547,   555,   562,   569,   575,   581,   589,   597,   604,
     611,   619,   626,   633,   641,   648,   655,   663,   670,   677,
     685,   693,   703,   704,   705,   706,   707,   708,   709,   710,
     714,   719,   727,   731,   739,   749,   758,   759,   760,   761,
     762,   763,   764,   765,   766,   767,   768,   769,   770,   771,
     772,   773,   774,   775,   776,   777,   778,   779,   780,   781,
     782,   783,   784,   785,   786,   787,   788,   789,   790,   791,
     792,   793,   794,   795,   796,   797,   798,   799,   800,   801,
     805,   812,   818,   827,   837,   852,   859,   864,   873,   886,
     901,   917,   929,   943,   956,   963,   970,   980,   990,   999,
    1015,  1019,  1027,  1028
  };

  // Print the state stack on the debug stream.
  void
  pgsParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  pgsParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "), ";
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  pgsParser::token_number_type
  pgsParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    98,     2,     2,
      92,    93,    96,    94,   101,    95,     2,    97,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   102,
      99,     2,   100,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    90,     2,    91,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   103,     2,   104,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int pgsParser::yyeof_ = 0;
  const int pgsParser::yylast_ = 784;
  const int pgsParser::yynnts_ = 31;
  const int pgsParser::yyempty_ = -2;
  const int pgsParser::yyfinal_ = 121;
  const int pgsParser::yyterror_ = 1;
  const int pgsParser::yyerrcode_ = 256;
  const int pgsParser::yyntokens_ = 105;

  const unsigned int pgsParser::yyuser_token_number_max_ = 344;
  const pgsParser::token_number_type pgsParser::yyundef_token_ = 2;

} // namespace pgscript

#line 1036 "pgscript/pgsParser.yy"
 /*** Additional Code ***/

void pgscript::pgsParser::error(const pgsParser::location_type & l,
		const std::string & m)
{
	wxLogScriptVerbose(wxT("EXPR STACK SIZE = %u"), driver.context.size_vars());
	wxLogScriptVerbose(wxT("STMT STACK SIZE = %u"), driver.context.size_stmts());
	driver.context.clear_stacks();
	driver.error(l, wxString(m.c_str(), wxConvUTF8));
}

