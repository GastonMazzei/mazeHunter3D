#!/bin/sh

usage ()
{
    echo "usage: $0 <testdir> <srcdir>" >&2
    if [ ! -z "$*" ]
    then
	echo "	$*" >&2
    fi
    exit 1
}

if [ $# -ne 2 ]
then usage
fi

# Chercher le fichier source C dans le répertoire. Il ne doit y en avoir
# qu'un seul.

TESTDIR="$1"
SRCDIR="$2"

# Conserver une trace de toutes les actions : fichier "log" dans le
# même répertoire que le source C.

LOG="$SRCDIR/log"
PROG="$SRCDIR/sender"
SERV="$SRCDIR/recver"
RETURN=0
rm -f "$LOG"

for TEST in $(ls "$TESTDIR"/test*.sh)
do
	# Extraire le numéro du test
    NUMTEST=$(echo $TEST | sed 's/.*-\([0-9][0-9]*\).sh/\1/')
    #on fout tout dans log
    echo "=========== Test $NUMTEST" >> "$LOG"
    #on lance le test en question, on redirige la sortie standard
    #vers log et l'erreur standard (2) vers la sortie standard (1)
    #'>>' append alors que '>' trunc et & permet de dire que c'est
    #un descripteur de fichier apres '>' seulemement
    #if test le retour du test
    echo "$TEST" "$TESTDIR" "$PROG" "$SERV"
    if bash "$TEST" "$TESTDIR" "$PROG" "$SERV" >> "$LOG" 2>&1
    then
		printf "Test $NUMTEST OK\n\n"
    else
		printf "Test $NUMTEST FAIL\n\n"
		RETURN=$(($RETURN+1))
    fi
done

exit $RETURN

