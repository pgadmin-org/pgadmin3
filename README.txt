pgAdmin III README
==================

Introduction
------------

This is an experimental version of pgAdmin, the popular PostgreSQL 
administration tool for Windows platforms (http://www.pgadmin.org).

As always, pgAdmin is being written to support the needs of the authors
and as a learning exercise. pgAdmin I was written in a very unstructured
manner using Visual Basic 6. When the limitations of the design became 
apparent, it was re-written from scratch using a very object oriented
approach, again in Visual Basic 6. This turned out to be much to the annoyance
of the many potential users that preferred *nix platforms.

pgAdmin III aims to replicate and improve the design of pgAdmin II, and 
support multiple platforms. After a long period of ongoing discussion, 
research and arguing, it was decided that the preferred development language
was C++, using the wxWindows (http://www.wxwindows.org) framework.

As it says above, this is a learning project as much as anything else and as
we are VB programmers, it may take a while so please bear with us as we teach
ourselves C++. Any comments or tips would be gratefully received!

Build Environment
-----------------

If you wish to build the current code just to take a look, or to help out with
the development, you will need:

1) wxWindows 2.4.0 + the StyledTextCtrl from /contrib + libwxxrc
2) A C++ compiler. We use GCC under Linux, and Microsoft Visual C++ under
   Windows XP.
3) libpq.

