#!/usr/bin/perl
##########################################################################
##
## pgAdmin III - Tab completion
## RCS-ID:      $Id$
## Copyright (C) 2002 - 2008, The pgAdmin Development Team
## This software is released under the Artistic Licence
##
## tab-complete.pl - Script to build tab-complete.inc 
##
##########################################################################


# This script builds tab-complete.inc from tab-complete.c. The
# source file (tab-complete.c) should be copied from the latest
# PostgreSQL sources (src/bin/psql/tab-complete.c), so we don't
# have to do all the work ourselves...

use strict;
use warnings;

open(F, "tab-complete.c") || die("Could not open tab-complete.c!");
undef $/;
my $alltxt = <F>;
close(F);

# Get rid of everything before and after the parts we're interested in
$alltxt =~ /.*(typedef struct SchemaQuery.*)\/\* GENERATOR FUNCTIONS\s+.*/s || die("Failed match 1");
$alltxt = $1;

# Get rid of forward declarations and initialize_readline
$alltxt =~ /(.*)\/\*\s+Forward declaration of functions \*\/.*(\/\* The completion function\..*)/s || die("Failed match 2");
$alltxt = $1 . $2;

# Get rid of completion macros, we define them ourselves
$alltxt =~ /(.*)\/\* A couple of macros to ease typing.*\*\/\s+#define COMPLETE_WITH_QUERY.*\s+(\/\*\s+\* Assembly instructions.*)/s || die("Failed match 4");
$alltxt = $1 . $2;

# Rewrite matches that don't use the macros
$alltxt =~ s/completion_matches\(text, create_command_generator\)/complete_create_command\(text\)/gs || die("Failed match 5");
$alltxt =~ s/completion_matches\(text, filename_completion_function\)/complete_filename\(\)/gs || die("Failed match 6");

# We're going to return char*, not char**
$alltxt =~ s/static char \*\*\s+psql_completion/static char \* psql_completion/s || die("Failed match 7");
$alltxt =~ s/char\s+\*\*matches = NULL/char \*matches = NULL/s || die("Failed match 8");

# Add an extra parameter to psql_completion
$alltxt =~ s/psql_completion\(char \*text, int start, int end\)/psql_completion\(char \*text, int start, int end, void \*dbptr\)/s || die("Failed match 9");

open(F,">tab-complete.inc") || die("Could not open tab-complete.inc!");
print F $alltxt;
print F "\n\n";
close(F);

print "Done.\n";
