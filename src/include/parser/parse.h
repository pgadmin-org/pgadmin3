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
     ADD = 262,
     AFTER = 263,
     AGGREGATE = 264,
     ALL = 265,
     ALTER = 266,
     ANALYSE = 267,
     ANALYZE = 268,
     AND = 269,
     ANY = 270,
     ARRAY = 271,
     AS = 272,
     ASC = 273,
     ASSERTION = 274,
     ASSIGNMENT = 275,
     AT = 276,
     AUTHORIZATION = 277,
     BACKWARD = 278,
     BEFORE = 279,
     BEGIN_P = 280,
     BETWEEN = 281,
     BIGINT = 282,
     BINARY = 283,
     BIT = 284,
     BOOLEAN_P = 285,
     BOTH = 286,
     BY = 287,
     CACHE = 288,
     CALLED = 289,
     CASCADE = 290,
     CASE = 291,
     CAST = 292,
     CHAIN = 293,
     CHAR_P = 294,
     CHARACTER = 295,
     CHARACTERISTICS = 296,
     CHECK = 297,
     CHECKPOINT = 298,
     CLASS = 299,
     CLOSE = 300,
     CLUSTER = 301,
     COALESCE = 302,
     COLLATE = 303,
     COLUMN = 304,
     COMMENT = 305,
     COMMIT = 306,
     COMMITTED = 307,
     CONSTRAINT = 308,
     CONSTRAINTS = 309,
     CONVERSION_P = 310,
     CONVERT = 311,
     COPY = 312,
     CREATE = 313,
     CREATEDB = 314,
     CREATEUSER = 315,
     CROSS = 316,
     CURRENT_DATE = 317,
     CURRENT_TIME = 318,
     CURRENT_TIMESTAMP = 319,
     CURRENT_USER = 320,
     CURSOR = 321,
     CYCLE = 322,
     DATABASE = 323,
     DAY_P = 324,
     DEALLOCATE = 325,
     DEC = 326,
     DECIMAL_P = 327,
     DECLARE = 328,
     DEFAULT = 329,
     DEFAULTS = 330,
     DEFERRABLE = 331,
     DEFERRED = 332,
     DEFINER = 333,
     DELETE_P = 334,
     DELIMITER = 335,
     DELIMITERS = 336,
     DESC = 337,
     DISTINCT = 338,
     DO = 339,
     DOMAIN_P = 340,
     DOUBLE_P = 341,
     DROP = 342,
     EACH = 343,
     ELSE = 344,
     ENCODING = 345,
     ENCRYPTED = 346,
     END_P = 347,
     ESCAPE = 348,
     EXCEPT = 349,
     EXCLUDING = 350,
     EXCLUSIVE = 351,
     EXECUTE = 352,
     EXISTS = 353,
     EXPLAIN = 354,
     EXTERNAL = 355,
     EXTRACT = 356,
     FALSE_P = 357,
     FETCH = 358,
     FIRST_P = 359,
     FLOAT_P = 360,
     FOR = 361,
     FORCE = 362,
     FOREIGN = 363,
     FORWARD = 364,
     FREEZE = 365,
     FROM = 366,
     FULL = 367,
     FUNCTION = 368,
     GLOBAL = 369,
     GRANT = 370,
     GROUP_P = 371,
     HANDLER = 372,
     HAVING = 373,
     HOLD = 374,
     HOUR_P = 375,
     ILIKE = 376,
     IMMEDIATE = 377,
     IMMUTABLE = 378,
     IMPLICIT_P = 379,
     IN_P = 380,
     INCLUDING = 381,
     INCREMENT = 382,
     INDEX = 383,
     INHERITS = 384,
     INITIALLY = 385,
     INNER_P = 386,
     INOUT = 387,
     INPUT_P = 388,
     INSENSITIVE = 389,
     INSERT = 390,
     INSTEAD = 391,
     INT_P = 392,
     INTEGER = 393,
     INTERSECT = 394,
     INTERVAL = 395,
     INTO = 396,
     INVOKER = 397,
     IS = 398,
     ISNULL = 399,
     ISOLATION = 400,
     JOIN = 401,
     KEY = 402,
     LANCOMPILER = 403,
     LANGUAGE = 404,
     LAST_P = 405,
     LEADING = 406,
     LEFT = 407,
     LEVEL = 408,
     LIKE = 409,
     LIMIT = 410,
     LISTEN = 411,
     LOAD = 412,
     LOCAL = 413,
     LOCALTIME = 414,
     LOCALTIMESTAMP = 415,
     LOCATION = 416,
     LOCK_P = 417,
     MATCH = 418,
     MAXVALUE = 419,
     MINUTE_P = 420,
     MINVALUE = 421,
     MODE = 422,
     MONTH_P = 423,
     MOVE = 424,
     NAMES = 425,
     NATIONAL = 426,
     NATURAL = 427,
     NCHAR = 428,
     NEW = 429,
     NEXT = 430,
     NO = 431,
     NOCREATEDB = 432,
     NOCREATEUSER = 433,
     NONE = 434,
     NOT = 435,
     NOTHING = 436,
     NOTIFY = 437,
     NOTNULL = 438,
     NULL_P = 439,
     NULLIF = 440,
     NUMERIC = 441,
     OF = 442,
     OFF = 443,
     OFFSET = 444,
     OIDS = 445,
     OLD = 446,
     ON = 447,
     ONLY = 448,
     OPERATOR = 449,
     OPTION = 450,
     OR = 451,
     ORDER = 452,
     OUT_P = 453,
     OUTER_P = 454,
     OVERLAPS = 455,
     OVERLAY = 456,
     OWNER = 457,
     PARTIAL = 458,
     PASSWORD = 459,
     PATH_P = 460,
     PENDANT = 461,
     PLACING = 462,
     POSITION = 463,
     PRECISION = 464,
     PRESERVE = 465,
     PREPARE = 466,
     PRIMARY = 467,
     PRIOR = 468,
     PRIVILEGES = 469,
     PROCEDURAL = 470,
     PROCEDURE = 471,
     READ = 472,
     REAL = 473,
     RECHECK = 474,
     REFERENCES = 475,
     REINDEX = 476,
     RELATIVE_P = 477,
     RENAME = 478,
     REPLACE = 479,
     RESET = 480,
     RESTART = 481,
     RESTRICT = 482,
     RETURNS = 483,
     REVOKE = 484,
     RIGHT = 485,
     ROLLBACK = 486,
     ROW = 487,
     ROWS = 488,
     RULE = 489,
     SCHEMA = 490,
     SCROLL = 491,
     SECOND_P = 492,
     SECURITY = 493,
     SELECT = 494,
     SEQUENCE = 495,
     SERIALIZABLE = 496,
     SESSION = 497,
     SESSION_USER = 498,
     SET = 499,
     SETOF = 500,
     SHARE = 501,
     SHOW = 502,
     SIMILAR = 503,
     SIMPLE = 504,
     SMALLINT = 505,
     SOME = 506,
     STABLE = 507,
     START = 508,
     STATEMENT = 509,
     STATISTICS = 510,
     STDIN = 511,
     STDOUT = 512,
     STORAGE = 513,
     STRICT_P = 514,
     SUBSTRING = 515,
     SYSID = 516,
     TABLE = 517,
     TEMP = 518,
     TEMPLATE = 519,
     TEMPORARY = 520,
     THEN = 521,
     TIME = 522,
     TIMESTAMP = 523,
     TO = 524,
     TOAST = 525,
     TRAILING = 526,
     TRANSACTION = 527,
     TREAT = 528,
     TRIGGER = 529,
     TRIM = 530,
     TRUE_P = 531,
     TRUNCATE = 532,
     TRUSTED = 533,
     TYPE_P = 534,
     UNENCRYPTED = 535,
     UNION = 536,
     UNIQUE = 537,
     UNKNOWN = 538,
     UNLISTEN = 539,
     UNTIL = 540,
     UPDATE = 541,
     USAGE = 542,
     USER = 543,
     USING = 544,
     VACUUM = 545,
     VALID = 546,
     VALIDATOR = 547,
     VALUES = 548,
     VARCHAR = 549,
     VARYING = 550,
     VERBOSE = 551,
     VERSION = 552,
     VIEW = 553,
     VOLATILE = 554,
     WHEN = 555,
     WHERE = 556,
     WITH = 557,
     WITHOUT = 558,
     WORK = 559,
     WRITE = 560,
     YEAR_P = 561,
     ZONE = 562,
     UNIONJOIN = 563,
     IDENT = 564,
     FCONST = 565,
     SCONST = 566,
     BCONST = 567,
     XCONST = 568,
     Op = 569,
     ICONST = 570,
     PARAM = 571,
     POSTFIXOP = 572,
     UMINUS = 573,
     TYPECAST = 574
   };
#endif
#define ABORT_P 258
#define ABSOLUTE_P 259
#define ACCESS 260
#define ACTION 261
#define ADD 262
#define AFTER 263
#define AGGREGATE 264
#define ALL 265
#define ALTER 266
#define ANALYSE 267
#define ANALYZE 268
#define AND 269
#define ANY 270
#define ARRAY 271
#define AS 272
#define ASC 273
#define ASSERTION 274
#define ASSIGNMENT 275
#define AT 276
#define AUTHORIZATION 277
#define BACKWARD 278
#define BEFORE 279
#define BEGIN_P 280
#define BETWEEN 281
#define BIGINT 282
#define BINARY 283
#define BIT 284
#define BOOLEAN_P 285
#define BOTH 286
#define BY 287
#define CACHE 288
#define CALLED 289
#define CASCADE 290
#define CASE 291
#define CAST 292
#define CHAIN 293
#define CHAR_P 294
#define CHARACTER 295
#define CHARACTERISTICS 296
#define CHECK 297
#define CHECKPOINT 298
#define CLASS 299
#define CLOSE 300
#define CLUSTER 301
#define COALESCE 302
#define COLLATE 303
#define COLUMN 304
#define COMMENT 305
#define COMMIT 306
#define COMMITTED 307
#define CONSTRAINT 308
#define CONSTRAINTS 309
#define CONVERSION_P 310
#define CONVERT 311
#define COPY 312
#define CREATE 313
#define CREATEDB 314
#define CREATEUSER 315
#define CROSS 316
#define CURRENT_DATE 317
#define CURRENT_TIME 318
#define CURRENT_TIMESTAMP 319
#define CURRENT_USER 320
#define CURSOR 321
#define CYCLE 322
#define DATABASE 323
#define DAY_P 324
#define DEALLOCATE 325
#define DEC 326
#define DECIMAL_P 327
#define DECLARE 328
#define DEFAULT 329
#define DEFAULTS 330
#define DEFERRABLE 331
#define DEFERRED 332
#define DEFINER 333
#define DELETE_P 334
#define DELIMITER 335
#define DELIMITERS 336
#define DESC 337
#define DISTINCT 338
#define DO 339
#define DOMAIN_P 340
#define DOUBLE_P 341
#define DROP 342
#define EACH 343
#define ELSE 344
#define ENCODING 345
#define ENCRYPTED 346
#define END_P 347
#define ESCAPE 348
#define EXCEPT 349
#define EXCLUDING 350
#define EXCLUSIVE 351
#define EXECUTE 352
#define EXISTS 353
#define EXPLAIN 354
#define EXTERNAL 355
#define EXTRACT 356
#define FALSE_P 357
#define FETCH 358
#define FIRST_P 359
#define FLOAT_P 360
#define FOR 361
#define FORCE 362
#define FOREIGN 363
#define FORWARD 364
#define FREEZE 365
#define FROM 366
#define FULL 367
#define FUNCTION 368
#define GLOBAL 369
#define GRANT 370
#define GROUP_P 371
#define HANDLER 372
#define HAVING 373
#define HOLD 374
#define HOUR_P 375
#define ILIKE 376
#define IMMEDIATE 377
#define IMMUTABLE 378
#define IMPLICIT_P 379
#define IN_P 380
#define INCLUDING 381
#define INCREMENT 382
#define INDEX 383
#define INHERITS 384
#define INITIALLY 385
#define INNER_P 386
#define INOUT 387
#define INPUT_P 388
#define INSENSITIVE 389
#define INSERT 390
#define INSTEAD 391
#define INT_P 392
#define INTEGER 393
#define INTERSECT 394
#define INTERVAL 395
#define INTO 396
#define INVOKER 397
#define IS 398
#define ISNULL 399
#define ISOLATION 400
#define JOIN 401
#define KEY 402
#define LANCOMPILER 403
#define LANGUAGE 404
#define LAST_P 405
#define LEADING 406
#define LEFT 407
#define LEVEL 408
#define LIKE 409
#define LIMIT 410
#define LISTEN 411
#define LOAD 412
#define LOCAL 413
#define LOCALTIME 414
#define LOCALTIMESTAMP 415
#define LOCATION 416
#define LOCK_P 417
#define MATCH 418
#define MAXVALUE 419
#define MINUTE_P 420
#define MINVALUE 421
#define MODE 422
#define MONTH_P 423
#define MOVE 424
#define NAMES 425
#define NATIONAL 426
#define NATURAL 427
#define NCHAR 428
#define NEW 429
#define NEXT 430
#define NO 431
#define NOCREATEDB 432
#define NOCREATEUSER 433
#define NONE 434
#define NOT 435
#define NOTHING 436
#define NOTIFY 437
#define NOTNULL 438
#define NULL_P 439
#define NULLIF 440
#define NUMERIC 441
#define OF 442
#define OFF 443
#define OFFSET 444
#define OIDS 445
#define OLD 446
#define ON 447
#define ONLY 448
#define OPERATOR 449
#define OPTION 450
#define OR 451
#define ORDER 452
#define OUT_P 453
#define OUTER_P 454
#define OVERLAPS 455
#define OVERLAY 456
#define OWNER 457
#define PARTIAL 458
#define PASSWORD 459
#define PATH_P 460
#define PENDANT 461
#define PLACING 462
#define POSITION 463
#define PRECISION 464
#define PRESERVE 465
#define PREPARE 466
#define PRIMARY 467
#define PRIOR 468
#define PRIVILEGES 469
#define PROCEDURAL 470
#define PROCEDURE 471
#define READ 472
#define REAL 473
#define RECHECK 474
#define REFERENCES 475
#define REINDEX 476
#define RELATIVE_P 477
#define RENAME 478
#define REPLACE 479
#define RESET 480
#define RESTART 481
#define RESTRICT 482
#define RETURNS 483
#define REVOKE 484
#define RIGHT 485
#define ROLLBACK 486
#define ROW 487
#define ROWS 488
#define RULE 489
#define SCHEMA 490
#define SCROLL 491
#define SECOND_P 492
#define SECURITY 493
#define SELECT 494
#define SEQUENCE 495
#define SERIALIZABLE 496
#define SESSION 497
#define SESSION_USER 498
#define SET 499
#define SETOF 500
#define SHARE 501
#define SHOW 502
#define SIMILAR 503
#define SIMPLE 504
#define SMALLINT 505
#define SOME 506
#define STABLE 507
#define START 508
#define STATEMENT 509
#define STATISTICS 510
#define STDIN 511
#define STDOUT 512
#define STORAGE 513
#define STRICT_P 514
#define SUBSTRING 515
#define SYSID 516
#define TABLE 517
#define TEMP 518
#define TEMPLATE 519
#define TEMPORARY 520
#define THEN 521
#define TIME 522
#define TIMESTAMP 523
#define TO 524
#define TOAST 525
#define TRAILING 526
#define TRANSACTION 527
#define TREAT 528
#define TRIGGER 529
#define TRIM 530
#define TRUE_P 531
#define TRUNCATE 532
#define TRUSTED 533
#define TYPE_P 534
#define UNENCRYPTED 535
#define UNION 536
#define UNIQUE 537
#define UNKNOWN 538
#define UNLISTEN 539
#define UNTIL 540
#define UPDATE 541
#define USAGE 542
#define USER 543
#define USING 544
#define VACUUM 545
#define VALID 546
#define VALIDATOR 547
#define VALUES 548
#define VARCHAR 549
#define VARYING 550
#define VERBOSE 551
#define VERSION 552
#define VIEW 553
#define VOLATILE 554
#define WHEN 555
#define WHERE 556
#define WITH 557
#define WITHOUT 558
#define WORK 559
#define WRITE 560
#define YEAR_P 561
#define ZONE 562
#define UNIONJOIN 563
#define IDENT 564
#define FCONST 565
#define SCONST 566
#define BCONST 567
#define XCONST 568
#define Op 569
#define ICONST 570
#define PARAM 571
#define POSTFIXOP 572
#define UMINUS 573
#define TYPECAST 574




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 101 "gram.y"
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
	FastList			fastlist;
	Node				*node;
	Value				*value;
	ColumnRef			*columnref;
	ObjectType			objtype;

	TypeName			*typnam;
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
#line 705 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



