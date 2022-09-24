#!/bin/bash

SCRIPT_REGEX=*.sh


if test $# -lt 1
then
	DIRECTORY=$PWD
elif

	if test $# -ne 1
	then
		echo "Wrong number of parameters. None or one directory must be passed as parameter";
		exit 1;
	fi
fi

if test -d $1
then
	echo "One directory must be passed as parameter" ; exit 1;
fi

DIRECTORY=$1

for $ELEMENT in $(find $DIRECTORY)

	
