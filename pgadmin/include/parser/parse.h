/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ABORT_P = 258,
     ABSOLUTE_P = 259,
     ACCESS = 260,
     ACTION = 261,
     ADD_P = 262,
     ADMIN = 263,
     AFTER = 264,
     AGGREGATE = 265,
     ALL = 266,
     ALSO = 267,
     ALTER = 268,
     ANALYSE = 269,
     ANALYZE = 270,
     AND = 271,
     ANY = 272,
     ARRAY = 273,
     AS = 274,
     ASC = 275,
     ASSERTION = 276,
     ASSIGNMENT = 277,
     ASYMMETRIC = 278,
     AT = 279,
     AUTHORIZATION = 280,
     BACKWARD = 281,
     BEFORE = 282,
     BEGIN_P = 283,
     BETWEEN = 284,
     BIGINT = 285,
     BINARY = 286,
     BIT = 287,
     BOOLEAN_P = 288,
     BOTH = 289,
     BY = 290,
     CACHE = 291,
     CALLED = 292,
     CASCADE = 293,
     CASCADED = 294,
     CASE = 295,
     CAST = 296,
     CHAIN = 297,
     CHAR_P = 298,
     CHARACTER = 299,
     CHARACTERISTICS = 300,
     CHECK = 301,
     CHECKPOINT = 302,
     CLASS = 303,
     CLOSE = 304,
     CLUSTER = 305,
     COALESCE = 306,
     COLLATE = 307,
     COLUMN = 308,
     COMMENT = 309,
     COMMIT = 310,
     COMMITTED = 311,
     CONCURRENTLY = 312,
     CONNECTION = 313,
     CONSTRAINT = 314,
     CONSTRAINTS = 315,
     CONVERSION_P = 316,
     CONVERT = 317,
     COPY = 318,
     CREATE = 319,
     CREATEDB = 320,
     CREATEROLE = 321,
     CREATEUSER = 322,
     CROSS = 323,
     CSV = 324,
     CURRENT_DATE = 325,
     CURRENT_ROLE = 326,
     CURRENT_TIME = 327,
     CURRENT_TIMESTAMP = 328,
     CURRENT_USER = 329,
     CURSOR = 330,
     CYCLE = 331,
     DATABASE = 332,
     DAY_P = 333,
     DEALLOCATE = 334,
     DEC = 335,
     DECIMAL_P = 336,
     DECLARE = 337,
     DEFAULT = 338,
     DEFAULTS = 339,
     DEFERRABLE = 340,
     DEFERRED = 341,
     DEFINER = 342,
     DELETE_P = 343,
     DELIMITER = 344,
     DELIMITERS = 345,
     DESC = 346,
     DISABLE_P = 347,
     DISTINCT = 348,
     DO = 349,
     DOMAIN_P = 350,
     DOUBLE_P = 351,
     DROP = 352,
     EACH = 353,
     ELSE = 354,
     ENABLE_P = 355,
     ENCODING = 356,
     ENCRYPTED = 357,
     END_P = 358,
     ESCAPE = 359,
     EXCEPT = 360,
     EXCLUDING = 361,
     EXCLUSIVE = 362,
     EXECUTE = 363,
     EXISTS = 364,
     EXPLAIN = 365,
     EXTERNAL = 366,
     EXTRACT = 367,
     FALSE_P = 368,
     FETCH = 369,
     FIRST_P = 370,
     FLOAT_P = 371,
     FOR = 372,
     FORCE = 373,
     FOREIGN = 374,
     FORWARD = 375,
     FREEZE = 376,
     FROM = 377,
     FULL = 378,
     FUNCTION = 379,
     GLOBAL = 380,
     GRANT = 381,
     GRANTED = 382,
     GREATEST = 383,
     GROUP_P = 384,
     HANDLER = 385,
     HAVING = 386,
     HEADER_P = 387,
     HOLD = 388,
     HOUR_P = 389,
     IF_P = 390,
     ILIKE = 391,
     IMMEDIATE = 392,
     IMMUTABLE = 393,
     IMPLICIT_P = 394,
     IN_P = 395,
     INCLUDING = 396,
     INCREMENT = 397,
     INDEX = 398,
     INDEXES = 399,
     INHERIT = 400,
     INHERITS = 401,
     INITIALLY = 402,
     INNER_P = 403,
     INOUT = 404,
     INPUT_P = 405,
     INSENSITIVE = 406,
     INSERT = 407,
     INSTEAD = 408,
     INT_P = 409,
     INTEGER = 410,
     INTERSECT = 411,
     INTERVAL = 412,
     INTO = 413,
     INVOKER = 414,
     IS = 415,
     ISNULL = 416,
     ISOLATION = 417,
     JOIN = 418,
     KEY = 419,
     LANCOMPILER = 420,
     LANGUAGE = 421,
     LARGE_P = 422,
     LAST_P = 423,
     LEADING = 424,
     LEAST = 425,
     LEFT = 426,
     LEVEL = 427,
     LIKE = 428,
     LIMIT = 429,
     LISTEN = 430,
     LOAD = 431,
     LOCAL = 432,
     LOCALTIME = 433,
     LOCALTIMESTAMP = 434,
     LOCATION = 435,
     LOCK_P = 436,
     LOGIN_P = 437,
     MATCH = 438,
     MAXVALUE = 439,
     MINUTE_P = 440,
     MINVALUE = 441,
     MODE = 442,
     MONTH_P = 443,
     MOVE = 444,
     NAMES = 445,
     NATIONAL = 446,
     NATURAL = 447,
     NCHAR = 448,
     NEW = 449,
     NEXT = 450,
     NO = 451,
     NOCREATEDB = 452,
     NOCREATEROLE = 453,
     NOCREATEUSER = 454,
     NOINHERIT = 455,
     NOLOGIN_P = 456,
     NONE = 457,
     NOSUPERUSER = 458,
     NOT = 459,
     NOTHING = 460,
     NOTIFY = 461,
     NOTNULL = 462,
     NOWAIT = 463,
     NULL_P = 464,
     NULLIF = 465,
     NUMERIC = 466,
     OBJECT_P = 467,
     OF = 468,
     OFF = 469,
     OFFSET = 470,
     OIDS = 471,
     OLD = 472,
     ON = 473,
     ONLY = 474,
     OPERATOR = 475,
     OPTION = 476,
     OR = 477,
     ORDER = 478,
     OUT_P = 479,
     OUTER_P = 480,
     OVERLAPS = 481,
     OVERLAY = 482,
     OWNED = 483,
     OWNER = 484,
     PARTIAL = 485,
     PASSWORD = 486,
     PLACING = 487,
     POSITION = 488,
     PRECISION = 489,
     PRESERVE = 490,
     PREPARE = 491,
     PREPARED = 492,
     PRIMARY = 493,
     PRIOR = 494,
     PRIVILEGES = 495,
     PROCEDURAL = 496,
     PROCEDURE = 497,
     QUOTE = 498,
     READ = 499,
     REAL = 500,
     REASSIGN = 501,
     RECHECK = 502,
     REFERENCES = 503,
     REINDEX = 504,
     RELATIVE_P = 505,
     RELEASE = 506,
     RENAME = 507,
     REPEATABLE = 508,
     REPLACE = 509,
     RESET = 510,
     RESTART = 511,
     RESTRICT = 512,
     RETURNING = 513,
     RETURNS = 514,
     REVOKE = 515,
     RIGHT = 516,
     ROLE = 517,
     ROLLBACK = 518,
     ROW = 519,
     ROWS = 520,
     RULE = 521,
     SAVEPOINT = 522,
     SCHEMA = 523,
     SCROLL = 524,
     SECOND_P = 525,
     SECURITY = 526,
     SELECT = 527,
     SEQUENCE = 528,
     SERIALIZABLE = 529,
     SESSION = 530,
     SESSION_USER = 531,
     SET = 532,
     SETOF = 533,
     SHARE = 534,
     SHOW = 535,
     SIMILAR = 536,
     SIMPLE = 537,
     SMALLINT = 538,
     SOME = 539,
     STABLE = 540,
     START = 541,
     STATEMENT = 542,
     STATISTICS = 543,
     STDIN = 544,
     STDOUT = 545,
     STORAGE = 546,
     STRICT_P = 547,
     SUBSTRING = 548,
     SUPERUSER_P = 549,
     SYMMETRIC = 550,
     SYSID = 551,
     SYSTEM_P = 552,
     TABLE = 553,
     TABLESPACE = 554,
     TEMP = 555,
     TEMPLATE = 556,
     TEMPORARY = 557,
     THEN = 558,
     TIME = 559,
     TIMESTAMP = 560,
     TO = 561,
     TRAILING = 562,
     TRANSACTION = 563,
     TREAT = 564,
     TRIGGER = 565,
     TRIM = 566,
     TRUE_P = 567,
     TRUNCATE = 568,
     TRUSTED = 569,
     TYPE_P = 570,
     UNCOMMITTED = 571,
     UNENCRYPTED = 572,
     UNION = 573,
     UNIQUE = 574,
     UNKNOWN = 575,
     UNLISTEN = 576,
     UNTIL = 577,
     UPDATE = 578,
     USER = 579,
     USING = 580,
     VACUUM = 581,
     VALID = 582,
     VALIDATOR = 583,
     VALUES = 584,
     VARCHAR = 585,
     VARYING = 586,
     VERBOSE = 587,
     VIEW = 588,
     VOLATILE = 589,
     WHEN = 590,
     WHERE = 591,
     WITH = 592,
     WITHOUT = 593,
     WORK = 594,
     WRITE = 595,
     YEAR_P = 596,
     ZONE = 597,
     WITH_CASCADED = 598,
     WITH_LOCAL = 599,
     WITH_CHECK = 600,
     IDENT = 601,
     FCONST = 602,
     SCONST = 603,
     BCONST = 604,
     XCONST = 605,
     Op = 606,
     ICONST = 607,
     PARAM = 608,
     POSTFIXOP = 609,
     UMINUS = 610,
     TYPECAST = 611
   };
#endif
#define ABORT_P 258
#define ABSOLUTE_P 259
#define ACCESS 260
#define ACTION 261
#define ADD_P 262
#define ADMIN 263
#define AFTER 264
#define AGGREGATE 265
#define ALL 266
#define ALSO 267
#define ALTER 268
#define ANALYSE 269
#define ANALYZE 270
#define AND 271
#define ANY 272
#define ARRAY 273
#define AS 274
#define ASC 275
#define ASSERTION 276
#define ASSIGNMENT 277
#define ASYMMETRIC 278
#define AT 279
#define AUTHORIZATION 280
#define BACKWARD 281
#define BEFORE 282
#define BEGIN_P 283
#define BETWEEN 284
#define BIGINT 285
#define BINARY 286
#define BIT 287
#define BOOLEAN_P 288
#define BOTH 289
#define BY 290
#define CACHE 291
#define CALLED 292
#define CASCADE 293
#define CASCADED 294
#define CASE 295
#define CAST 296
#define CHAIN 297
#define CHAR_P 298
#define CHARACTER 299
#define CHARACTERISTICS 300
#define CHECK 301
#define CHECKPOINT 302
#define CLASS 303
#define CLOSE 304
#define CLUSTER 305
#define COALESCE 306
#define COLLATE 307
#define COLUMN 308
#define COMMENT 309
#define COMMIT 310
#define COMMITTED 311
#define CONCURRENTLY 312
#define CONNECTION 313
#define CONSTRAINT 314
#define CONSTRAINTS 315
#define CONVERSION_P 316
#define CONVERT 317
#define COPY 318
#define CREATE 319
#define CREATEDB 320
#define CREATEROLE 321
#define CREATEUSER 322
#define CROSS 323
#define CSV 324
#define CURRENT_DATE 325
#define CURRENT_ROLE 326
#define CURRENT_TIME 327
#define CURRENT_TIMESTAMP 328
#define CURRENT_USER 329
#define CURSOR 330
#define CYCLE 331
#define DATABASE 332
#define DAY_P 333
#define DEALLOCATE 334
#define DEC 335
#define DECIMAL_P 336
#define DECLARE 337
#define DEFAULT 338
#define DEFAULTS 339
#define DEFERRABLE 340
#define DEFERRED 341
#define DEFINER 342
#define DELETE_P 343
#define DELIMITER 344
#define DELIMITERS 345
#define DESC 346
#define DISABLE_P 347
#define DISTINCT 348
#define DO 349
#define DOMAIN_P 350
#define DOUBLE_P 351
#define DROP 352
#define EACH 353
#define ELSE 354
#define ENABLE_P 355
#define ENCODING 356
#define ENCRYPTED 357
#define END_P 358
#define ESCAPE 359
#define EXCEPT 360
#define EXCLUDING 361
#define EXCLUSIVE 362
#define EXECUTE 363
#define EXISTS 364
#define EXPLAIN 365
#define EXTERNAL 366
#define EXTRACT 367
#define FALSE_P 368
#define FETCH 369
#define FIRST_P 370
#define FLOAT_P 371
#define FOR 372
#define FORCE 373
#define FOREIGN 374
#define FORWARD 375
#define FREEZE 376
#define FROM 377
#define FULL 378
#define FUNCTION 379
#define GLOBAL 380
#define GRANT 381
#define GRANTED 382
#define GREATEST 383
#define GROUP_P 384
#define HANDLER 385
#define HAVING 386
#define HEADER_P 387
#define HOLD 388
#define HOUR_P 389
#define IF_P 390
#define ILIKE 391
#define IMMEDIATE 392
#define IMMUTABLE 393
#define IMPLICIT_P 394
#define IN_P 395
#define INCLUDING 396
#define INCREMENT 397
#define INDEX 398
#define INDEXES 399
#define INHERIT 400
#define INHERITS 401
#define INITIALLY 402
#define INNER_P 403
#define INOUT 404
#define INPUT_P 405
#define INSENSITIVE 406
#define INSERT 407
#define INSTEAD 408
#define INT_P 409
#define INTEGER 410
#define INTERSECT 411
#define INTERVAL 412
#define INTO 413
#define INVOKER 414
#define IS 415
#define ISNULL 416
#define ISOLATION 417
#define JOIN 418
#define KEY 419
#define LANCOMPILER 420
#define LANGUAGE 421
#define LARGE_P 422
#define LAST_P 423
#define LEADING 424
#define LEAST 425
#define LEFT 426
#define LEVEL 427
#define LIKE 428
#define LIMIT 429
#define LISTEN 430
#define LOAD 431
#define LOCAL 432
#define LOCALTIME 433
#define LOCALTIMESTAMP 434
#define LOCATION 435
#define LOCK_P 436
#define LOGIN_P 437
#define MATCH 438
#define MAXVALUE 439
#define MINUTE_P 440
#define MINVALUE 441
#define MODE 442
#define MONTH_P 443
#define MOVE 444
#define NAMES 445
#define NATIONAL 446
#define NATURAL 447
#define NCHAR 448
#define NEW 449
#define NEXT 450
#define NO 451
#define NOCREATEDB 452
#define NOCREATEROLE 453
#define NOCREATEUSER 454
#define NOINHERIT 455
#define NOLOGIN_P 456
#define NONE 457
#define NOSUPERUSER 458
#define NOT 459
#define NOTHING 460
#define NOTIFY 461
#define NOTNULL 462
#define NOWAIT 463
#define NULL_P 464
#define NULLIF 465
#define NUMERIC 466
#define OBJECT_P 467
#define OF 468
#define OFF 469
#define OFFSET 470
#define OIDS 471
#define OLD 472
#define ON 473
#define ONLY 474
#define OPERATOR 475
#define OPTION 476
#define OR 477
#define ORDER 478
#define OUT_P 479
#define OUTER_P 480
#define OVERLAPS 481
#define OVERLAY 482
#define OWNED 483
#define OWNER 484
#define PARTIAL 485
#define PASSWORD 486
#define PLACING 487
#define POSITION 488
#define PRECISION 489
#define PRESERVE 490
#define PREPARE 491
#define PREPARED 492
#define PRIMARY 493
#define PRIOR 494
#define PRIVILEGES 495
#define PROCEDURAL 496
#define PROCEDURE 497
#define QUOTE 498
#define READ 499
#define REAL 500
#define REASSIGN 501
#define RECHECK 502
#define REFERENCES 503
#define REINDEX 504
#define RELATIVE_P 505
#define RELEASE 506
#define RENAME 507
#define REPEATABLE 508
#define REPLACE 509
#define RESET 510
#define RESTART 511
#define RESTRICT 512
#define RETURNING 513
#define RETURNS 514
#define REVOKE 515
#define RIGHT 516
#define ROLE 517
#define ROLLBACK 518
#define ROW 519
#define ROWS 520
#define RULE 521
#define SAVEPOINT 522
#define SCHEMA 523
#define SCROLL 524
#define SECOND_P 525
#define SECURITY 526
#define SELECT 527
#define SEQUENCE 528
#define SERIALIZABLE 529
#define SESSION 530
#define SESSION_USER 531
#define SET 532
#define SETOF 533
#define SHARE 534
#define SHOW 535
#define SIMILAR 536
#define SIMPLE 537
#define SMALLINT 538
#define SOME 539
#define STABLE 540
#define START 541
#define STATEMENT 542
#define STATISTICS 543
#define STDIN 544
#define STDOUT 545
#define STORAGE 546
#define STRICT_P 547
#define SUBSTRING 548
#define SUPERUSER_P 549
#define SYMMETRIC 550
#define SYSID 551
#define SYSTEM_P 552
#define TABLE 553
#define TABLESPACE 554
#define TEMP 555
#define TEMPLATE 556
#define TEMPORARY 557
#define THEN 558
#define TIME 559
#define TIMESTAMP 560
#define TO 561
#define TRAILING 562
#define TRANSACTION 563
#define TREAT 564
#define TRIGGER 565
#define TRIM 566
#define TRUE_P 567
#define TRUNCATE 568
#define TRUSTED 569
#define TYPE_P 570
#define UNCOMMITTED 571
#define UNENCRYPTED 572
#define UNION 573
#define UNIQUE 574
#define UNKNOWN 575
#define UNLISTEN 576
#define UNTIL 577
#define UPDATE 578
#define USER 579
#define USING 580
#define VACUUM 581
#define VALID 582
#define VALIDATOR 583
#define VALUES 584
#define VARCHAR 585
#define VARYING 586
#define VERBOSE 587
#define VIEW 588
#define VOLATILE 589
#define WHEN 590
#define WHERE 591
#define WITH 592
#define WITHOUT 593
#define WORK 594
#define WRITE 595
#define YEAR_P 596
#define ZONE 597
#define WITH_CASCADED 598
#define WITH_LOCAL 599
#define WITH_CHECK 600
#define IDENT 601
#define FCONST 602
#define SCONST 603
#define BCONST 604
#define XCONST 605
#define Op 606
#define ICONST 607
#define PARAM 608
#define POSTFIXOP 609
#define UMINUS 610
#define TYPECAST 611




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 116 "gram.y"
typedef union YYSTYPE {
	int					ival;
	char				chr;
	char				*str;
	const char			*keyword;
	bool				boolean;
	JoinType			jtype;
	DropBehavior		dbehavior;
	OnCommitAction		oncommit;
	List				*list;
	Node				*node;
	Value				*value;
	ObjectType			objtype;

	TypeName			*typnam;
	FunctionParameter   *fun_param;
	FunctionParameterMode fun_param_mode;
	FuncWithArgs		*funwithargs;
	DefElem				*defelt;
	SortBy				*sortby;
	JoinExpr			*jexpr;
	IndexElem			*ielem;
	Alias				*alias;
	RangeVar			*range;
	A_Indices			*aind;
	ResTarget			*target;
	PrivTarget			*privtarget;

	InsertStmt			*istmt;
	VariableSetStmt		*vsetstmt;
} YYSTYPE;
/* Line 1248 of yacc.c.  */
#line 780 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE base_yylval;

#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE base_yylloc;


