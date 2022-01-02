#!/bin/sh

if [ $# -ne 3 ]
then
    echo "usage: $0 <testdir> <program-to-test> <server-to-test>" >&2
    exit 1
fi

#Test arguments

TESTDIR="$1"
PROG="$2"
SERV="$3"
TMP="tmp"
LOG="log"
DATA="$TESTDIR/datatest1"
TEST_CRASH=0
. "$TESTDIR"/biblio.sh

ERROR_STR="Client | Syntaxe invalide : pas d'argument"
$PROG 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	if [ $(test_rerror $TMP) ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
		cat $TMP >> $LOG
	fi
fi

ERROR_STR="Serveur | Syntaxe invalide : pas d'argument"
$SERV 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	if [ $(test_rerror $TMP) ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
		cat $TMP >> $LOG
	fi
fi

ERROR_STR="Client | Syntaxe invalide : trop d'arguments"
$PROG "moodle3.unistra.fr" b c d e f g 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	if [ $(test_rerror $TMP) ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
		cat $TMP >> $LOG
	fi
fi

ERROR_STR="Serveur | Syntaxe invalide : trop d'arguments"
$SERV "moodle3.unistra.fr" b c d e f g 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	test_rerror $TMP
	if [ $(test_rerror $TMP) ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
		cat $TMP >> $LOG
	fi
fi

ERROR_STR="Client | Syntaxe invalide : bad arguments 1"
$PROG a "$DATA/bddsites" 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	if [ $(test_rerror $TMP) ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
		cat $TMP >> $LOG
	fi
fi

ERROR_STR="Client | Syntaxe invalide : bad arguments 2"
$PROG "$DATA/bddclient" a 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	if [ $(test_rerror $TMP) ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
		cat $TMP >> $LOG
	fi
fi

ERROR_STR="Serveur | Syntaxe invalide : bad arguments 1"
$SERV 1 "$DATA/bddserv1" 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	test_rerror $TMP
	if [ $(test_rerror $TMP) ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
		cat $TMP >> $LOG
	fi
fi

ERROR_STR="Serveur | Syntaxe invalide : bad arguments 2"
$SERV 3500 a 2> $TMP
if [ ! $? ]
then
	echo $ERROR_STR
	TEST_CRASH=$((TEST_CRASH + 1))
else
	test_rerror $TMP
	if [ $(test_rerror $TMP) ]
	then
		echo $ERROR_STR
		TEST_CRASH=$((TEST_CRASH + 1))
		cat $TMP >> $LOG
	fi
fi

if [ $TEST_CRASH -gt 0 ]
then
	echo "nb crash : $TEST_CRASH\n"
fi

clean_tmp
exit $TEST_CRASH

