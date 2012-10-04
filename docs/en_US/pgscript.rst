.. _pgscript:


**********************************************
`pgScript Scripting Language Reference`:index:
**********************************************


.. _pgscript-overview:

Overview
========

pgScript is composed of pgScript commands::

   pgScript command
       : Regular PostgreSQL SQL Command (SELECT INSERT CREATE ...)
       | Variable declaration or assignment (DECLARE SET)
       | Control-of-flow structure (IF WHILE)
       | Procedure (ASSERT PRINT LOG RMLINE)

Command names (**SELECT**, **IF**, **SET**, ...) are case-insensitive
and must be ended with a semi-column ``;``. Identifiers are case-sensitive.

.. _examples:

Examples
========

.. _example1:

Batch table creations
---------------------

::

   DECLARE @I, @T; -- Variable names begin with a @
   SET @I = 0; -- @I is an integer
   WHILE @I &lt; 20
   BEGIN
      SET @T = 'table' + CAST (@I AS STRING); -- Casts @I
      CREATE TABLE @T (id integer primary key, data text);
      
      SET @I = @I + 1;
   END

.. _example2:

Insert random data
------------------

::

   DECLARE @I, @J, @T, @G;
   SET @I = 0;
   SET @G1 = INTEGER(10, 29, 1); /* Random integer generator
                                Unique numbers between 10 and 29 */
   SET @G2 = STRING(10, 20, 3); /* Random string generator
                                3 words between 10 and 20 characters */
   WHILE @I &lt; 20
   BEGIN
       SET @T = 'table' + CAST (@I AS STRING);
   
   SET @J = 0;
       WHILE @J &lt; 20
       BEGIN
           INSERT INTO @T VALUES (@G1, '@G2');
           SET @J = @J + 1;
       END
   
   SET @I = @I + 1;
   END

.. _example3:

Batch table deletions
---------------------

::

   DECLARE @I, @T; -- Declaring is optional
   SET @I = 0;
   WHILE 1 -- Always true
   BEGIN
       IF @I &gt;= 20
         BREAK; -- Exit the loop if @I &gt; 20
    
    SET @T = 'table' + CAST (@I AS STRING);
       DROP TABLE @T;
   
    SET @I = @I + 1;
   END

.. _example4:

Print information on screen
---------------------------

::

   SET @PROGR@M#TITLE = 'pgScript';
   PRINT '';
   PRINT @PROGR@M#TITLE + ' features:';
   PRINT '';
   PRINT '  * Regular PostgreSQL commands';
   PRINT '  * Control-of-flow language';
   PRINT '  * Local variables';
   PRINT '  * Random data generators';</pre>

.. _commands:

SQL Commands
============

You can run ANY PostgreSQL query from a pgScript EXCEPT those ones::

   BEGIN;
   END;

This is because **BEGIN** and **END** are used for delimiting blocks.
Instead use::

   BEGIN TRANSACTION;
   END TRANSACTION;

For a list of PostgreSQL commands: `http://www.postgresql.org/docs/8.3/interactive/sql-commands.html <http://www.postgresql.org/docs/8.3/interactive/sql-commands.html>`_

.. _variables:

Variables
=========

There are two main types of variables : simple variables and records
(result sets composed of lines and columns).

Variable names begin with a ``@`` and can be composed of
letters, digits, ``_``, ``#``, ``@``.

Variable type is guessed automatically according to the kind of value it
contains. This can be one of: number (real or integer), string, record.

.. _variable1:

Simple variables
----------------

Simple variable declaration
***************************

Declaring simple variable is optional::

   DECLARE @A, @B;
   DECLARE @VAR1;

Simple variable affectation
***************************

This is done with the **SET** command. The variable type depends on the
value assigned to this variable::

   SET @A = 1000, @B = 2000;   -- @A and @B are <strong>integer numbers**
   SET @C = 10e1, @D = 1.5;    -- @C and @D are <strong>real numbers**
   SET @E = 'ab', @F = 'a''b'; -- @E and @F are <strong>strings**
   SET @G = "ab", @H = "a\"b"; -- @G and @H are <strong>strings**

An uninitialized variable defaults to an empty string. It is possible to
override variables as many times as wanted::

   PRINT @A;      -- Prints an empty string
   SET @A = 1000; -- @A is initialized an integer
   PRINT @A;      -- Prints 1000
   SET @A = 'ab'; -- @A becomes a string
   PRINT @A;      -- Prints ab

Data generators
***************

Data generators allows users to generate random values. There are
various types of generators, each one producing different type of data.
A variable initialized with a data generator behaves like a regular
simple variable except that it has a different value each time it is
used::

   SET @A = INTEGER(100, 200);
   PRINT @A; -- Prints an integer between 100 and 200
   PRINT @A; -- Prints another integer between 100 and 200

A variable can contain a generator but its type is one of: number (real
or integer), string. For a list of available generators and their
associated type, see `generators`_.

.. _variable2:

Records
-------

Record declaration
******************

Declaring a record is **required**. A name for each column must be
specified even if they will not be used anymore afterwards::

   DECLARE @R1 { @A, @B }, @R2 { @A, @C }; -- Two records with two  columns
   DECLARE @R3 { @A, @B, @C, @D };         -- One record  with four columns

The number of lines is dynamic: see the next section.

Record affectation
******************

To access a specific location in a record, one must use the line number
(starts at 0) and can use either the column name (between quotes) or the
column number (starts at 0). This specific location behaves like a
simple variable. Note that a record cannot contain a record::

   SET @R1[0]['@A'] = 1; -- First line &amp; first column
   SET @R1[0][0] = 1;    -- Same location
   SET @R1[4]['@B'] = 1; -- Fifth line &amp; second column
   SET @R1[0][1] = 1;    -- Same location

In the above example, three empty lines are automatically inserted
between the first and the fifth. Using an invalid column number or name
results in an exception.

Specific location can be used as right values as well. A specific line
can also be used as right value::

   SET @R1[0][0] = @R3[0][1], @A = @R2[0][0]; -- Behaves like simple variables
   SET @A = @R1[1]; -- @A becomes a record which is the first line of @R1

Remember that ``SET @R1[0][0] = @R2`` is impossible because a record
cannot contain a record.

It is possible to assign a record to a variable, in this case the
variable does not need to be declared::

   SET @A = @R3; -- @A becomes a record because it is assigned a record

SQL queries
***********

Any SQL query executed returns a record. If the query is a ``SELECT``
query then it returns the results of the query. If it is something else
then it returns a one-line record (``true``) if this is a success
otherwise a zero-line record (``false``)::

   SET @A = SELECT * FROM table;   -- @A is a record with the results of the query
   SET @B = INSERT INTO table ...; -- @B is a one-line record if the query succeeds

Record functions
****************

See `function2`_.

.. _variable3:

Cast
----

It is possible to convert a variable from one type to another with the
cast function::

   SET @A = CAST (@B AS STRING);
   SET @A = CAST (@B AS REAL);
   SET @A = CAST (@B AS INTEGER);
   SET @A = CAST (@B AS RECORD);

When a record is converted to a string, it is converted to its flat
representation. When converted to a number, the record is first converted
to a string and then to a number (see string conversion for more
details).

When a number is converted to a string, it is converted to its string
representation. When converted to a record, it is converted to a
one-line-one-column record whose value is the number.

When a string is converted to a number, if the string represents a
number then this number is returned else an exception is thrown. When
converted to a record, either the program can find a **record pattern**
in the string or it converts it to a one-line-one-column record whose
value is the string. A record pattern is::

   SET @B = '(1, "abc", "ab\\"")(1, "abc", "ab\\"")'; -- @B is a string
   SET @B = CAST (@B AS RECORD); @B becomes a two-line-three-column record

Remember a string is surrounded by simple quotes. Strings composing a
record must be surrounded by double quotes which are escaped with ``\\``
(we double the slash because it is already a special character for the
enclosing simple quotes).

.. _variable4:

Operations
----------

Operations can only be performed between operands of the same type. Cast
values in order to conform to this criterion.

Comparisons result in a number which is 0 or 1.

Strings
*******

Comparisons: ``= <> > < <= >= AND OR``

Concatenation: ``+``

::

   SET @B = @A + 'abcdef'; -- @A must be a string and @B will be a string

Boolean value: non-empty string is ``true``, empty string is ``false``

Inverse boolean value: ``NOT``

Case-insensitive comparison: ``~=``

Numbers
*******

Comparisons: ``= <> > < <= >= AND OR``

Arithmetic: ``+ - * / %``

::

   SET @A = CAST ('10' AS INTEGER) + 5; -- '10' string is converted to a number

Boolean value: 0 is ``false``, anything else is ``true``

Inverse boolean value: ``NOT`` (note that ``NOT NOT 10 = 1``)

An arithmetic operation involving at least one real number gives a real
number as a result::

   SET @A = 10 / 4.; -- 4. is a real so real division: @A = 2.5
   SET @A = 10 / 4;  -- 4 is an integer so integer division: @A = 2

Records
*******

Comparisons: ``= <> > < <= >= AND OR``

Boolean value: zero-line record is ``false``, anything else is ``true``

Inverse boolean value: ``NOT``

Comparisons for records are about inclusion and exclusion. Order of
lines does not matter. ``<=`` means that each row in the left operand
has a match in the right operand. ``>=`` means the opposite. ``=`` means
that ``<=`` and ``>=`` are both true at the same time...

Comparisons are performed on strings: even if a record contains numbers
like ``10`` and ``1e1`` we will have ``'10' <> '1e1'``.

.. _control:

Control-of-flow structures
==========================

.. _control1:

Conditional structure
---------------------

::

   IF condition
   BEGIN
       pgScript commands
   END
   ELSE
   BEGIN
       pgScript commands
   END

pgScript commands are optional. **BEGIN** and **END** keywords are
optional if there is only one pgScript command.

.. _control2:

Loop structure
--------------

::

   WHILE condition
   BEGIN
       pgScript commands
   END

pgScript commands are optional. **BEGIN** and **END** keywords are
optional if there is only one pgScript command.

**BREAK** ends the enclosing **WHILE** loop, while **CONTINUE** causes
the next iteration of the loop to execute. **RETURN** behaves like
**BREAK**::

   WHILE condition1
   BEGIN
       IF condition2
       BEGIN
           BREAK;
       END
   END

.. _control3:

Conditions
----------

Conditions are in fact results of operations. For example the string
comparison ``'ab' = 'ac'`` will result in a number which is ``false``
(the equality is not true)::

   IF 'ab' ~= 'AB' -- Case-insensitive comparison which result in 1 (true) which is true
   BEGIN
       -- This happens
   END
   
   IF 0 -- false
   BEGIN
       -- This does **not** happen
   END
   ELSE
   BEGIN
       -- This happens 
   END
   
   WHILE 1
   BEGIN
       -- Infinite loop: use BREAK for exiting
   END

It is possible to the result of a SQL SELECT query directly as a
condition. The query needs to be surrounded by parenthesis::

   IF (SELECT 1 FROM table)
   BEGIN
       -- This means that table exists otherwise the condition would be false
   END

.. _functions:

Additional functions and procedures
===================================

.. _function1:

Procedures
----------

Procedures do not return a result. They must be used alone on a line and
cannot be assigned to a variable.

Print
*****

Prints an expression on the screen::

   PRINT 'The value of @A is' + CAST (@A AS STRING);

Assert
******

Throws an exception if the expression evaluated is false::

   ASSERT 5 &gt; 3 AND 'a' = 'a';

Remove line
***********

Removes the specified line of a record::

   RMLINE(@R[1]); -- Removes @R second line

.. _function2:

Functions
---------

Functions do return a result. Their return value can be assigned to a
variable, like the ``CAST`` operation.

Trim
****

Removes extra spaces surrounding a string::

   SET @A = TRIM(' a '); -- @A = 'a'</pre>

Lines
*****

Gives the number of lines in a record::

   IF LINES(@R) &gt; 0
   BEGIN
       -- Process
   END

Columns
*******

Gives the number of columns in a record::

   IF COLUMNS(@R) &gt; 0
   BEGIN
       -- Process
   END

.. _generators:

Random data generators
======================

.. _generator1:

Overview of the generators
--------------------------

One can assign a variable (**SET**) with a random data generators. This
means each time the variable will be used it will have a different
value.

However the variable is still used as usual::

   SET @G = STRING(10, 20, 2);
   SET @A = @G; -- @A will hold a random string
   SET @B = @G; -- @B will hold another random string
   PRINT @G,    -- This will print another third random string

.. _generator2:

Sequence and seeding
--------------------

Common parameters for data generators are *sequence* and *seed*.

*sequence* means that a sequence of values is generated in a random
order, in other words each value appears only once before the sequence
starts again: this is useful for columns with a ``UNIQUE`` constraint.
For example, this generator::

   SET @G = INTEGER(10, 15, 1); -- 1 means generate a sequence

It can generate such values::

   14 12 10 13 11 15 14 12 10 13 11

Where each number appears once before the sequence starts repeating.

*sequence* parameter must be an integer: if it is 0 then no sequence is
generated (default) and if something other than 0 then generate a
sequence.

*seed* is an integer value for initializing a generator: two generators
with the same parameters and the same seed will generate **exactly** the
same values.

*seed* must be an integer: it is used directly to initialize the random
data generator.

.. _generator3:

Data generators
---------------

Optional parameters are put into brackets::

   Generator
    : INTEGER ( min, max, [sequence], [seed] );
    | REAL ( min, max, precision, [sequence], [seed] );
    | DATE ( min, max, [sequence], [seed] );
    | TIME ( min, max, [sequence], [seed] );
    | DATETIME ( min, max, [sequence], [seed] );
    | STRING ( min, max, [nb], [seed] );
    | REGEX ( regex, [seed] );
    | FILE ( path, [sequence], [seed], [encoding] );
    | REFERENCE ( table, column, [sequence], [seed] );

Integer numbers
***************

::

   INTEGER ( min, max, [sequence], [seed] );
   INTEGER ( -10, 10, 1, 123456 );

``min`` is an integer, ``max`` is an integer, ``sequence`` is an integer
and ``seed`` is an integer.

Real numbers
************

::

   REAL ( min, max, precision, [sequence], [seed] );
   REAL ( 1.5, 1.8, 2, 1 );

``min`` is a number, ``max`` is a number, ``precision`` is an integer
that indicates the number of decimals (should be less than 30),
``sequence`` is an integer and ``seed`` is an integer.

Dates
*****

::

   DATE ( min, max, [sequence], [seed] );
   DATE ( '2008-05-01', '2008-05-05', 0 );

``min`` is a string representing a date, ``max`` is a string representing
a date, ``sequence`` is an integer and ``seed`` is an integer.

Times
*****

::

   TIME ( min, max, [sequence], [seed] );
   TIME ( '00:30:00', '00:30:15', 0 );

``min`` is a string representing a time, ``max`` is a string representing
a time, ``sequence`` is an integer and ``seed`` is an integer.

Timestamps (date/times)
***********************

::

   DATETIME ( min, max, [sequence], [seed] );
   DATETIME ( '2008-05-01 14:00:00', '2008-05-05 15:00:00', 1 );

``min`` is a string representing a timestamp, ``max`` is a string
representing a timestamp, ``sequence`` is an integer and ``seed`` is an
integer.

Strings
*******

::

   STRING ( min, max, [nb], [seed] );
   STRING ( 10, 20, 5 );

``min`` is an integer representing the minimum length of a word, ``max``
is an integer representing the maximum length of a word, ``nb`` is an
integer representing the number of words (default: ``1``) and ``seed``
is an integer.

In the above example we generate 5 words (separated with a space) whose
size is between 10 and 20 characters.

Strings from regular expressions
********************************

::

   REGEX ( regex, [seed] );
   REGEX ( '[a-z]{1,3}@[0-9]{3}' );

``regex`` is a string representing a simplified regular expressions and
``seed`` is an integer.

Simplified regular expressions are composed of:

* Sets of possible characters like ``[a-z_.]`` for characters between
  ``a`` and ``z`` + ``_`` and ``.``
* Single characters

It is possible to specify the minimum and maximum length of the
preceding set or single character:

* ``{min, max}`` like ``{1,3}`` which stands for length between ``1``
  and ``3``
* ``{min}`` like ``{3}`` which stands for length of ``3``
* Default (when nothing is specified) is length of ``1``

Note: be careful with spaces because ``'a {3}'`` means one ``a`` followed
by three spaces because the ``3`` is about the last character or set of
characters which is a space in this example.

If you need to use ``[`` ``]`` ``\`` ``{`` or ``}``, they must be escaped
because they are special characters. Remember to use **double
backslash**: ``'\\[{3}'`` for three ``[``.

Strings from dictionary files
*****************************

::

   FILE ( path, [sequence], [seed], [encoding] );
   FILE ( 'file.txt', 0, 54321, 'utf-8' );

``path`` is a string representing the path to a text file, ``sequence``
is an integer, ``seed`` is an integer and ``encoding`` is a string
representing the file character set (default is system encoding).

This generates a random integer between 1 and the number of lines in the
file and then returns that line. If the file does not exist then an
exception is thrown.

``encoding`` supports the most known encoding like utf-8, utf-16le,
utf-16be, iso-8859-1, ...

Reference to another field
**************************

::

   REFERENCE ( table, column, [sequence], [seed] );
   REFERENCE ( 'tab', 'col', 1 );

``table`` is a string representing a table, ``column`` is a string
representing a column of the table, ``sequence`` is an integer and
``seed`` is an integer.

This is useful for generating data to put into foreign-key-constrained
columns.
