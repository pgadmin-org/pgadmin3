#!/bin/sh

#############
## Options ##
#############

# Default parameters for pgScript if nothing is provided (see Usage).
DEFAULTPARAM="-h 127.0.0.1 -U postgres -d testbase"

##################
## Main program ##
##################

echo "Runs integration tests"
echo "Usage: $0 [parameters for db connection]"
echo ""

THISDIR=`dirname $0`
PREVDIR="$PWD"

echo -n "cd $THISDIR... "
cd "$THISDIR"
echo "done"
echo ""

# Get parameters for database connections
if test "$*" == ""
then
	PARAMETERS="$DEFAULTPARAM" # Default if nothing is provided
else
	PARAMETERS="$*"
fi
echo -e "- Using '$PARAMETERS' \n  for database connection"

# Try to find pgScript
PGS1="./../../src/pgScript"
PGS2="./../../bin/pgScript"
if test -f "$PGS1"
then
	PGS="$PGS1"
else
	if test -f "$PGS2"
	then
		PGS="$PGS2"
	else
		echo "- pgScript executable not found: please configure script"
		exit
	fi
fi
echo "- pgScript at '$PGS' found"
echo "- Check output below: no [EXCEPTION] should appear."
echo ""

# Execute pgScript programs
echo "############# RUNNING  TESTS #############"

for src in `ls ./source/*.sql.pgs`; do
	output=$($PGS $PARAMETERS -e cp1252 "$src" 2>&1 >/dev/null)
	echo "$output" | awk '{
		if ($1 == "[QUERY")
		{
			query=1
		}
		else if ($1 != "[QUERY" && query == 0)
		{
			print $0
			query=0
		}}'
done
echo "################## DONE ##################"

echo ""
echo -n "cd $PREVDIR... "
cd "$PREVDIR"
echo "done"
