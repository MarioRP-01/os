#!/bin/bash

SCRIPT_REGEX=*.sh

if test $# -gt 1
then
	echo "Wrong number of parameters" ; exit 1;
fi

if test $# -eq 0
then 
	DIR=$PWD
elif test -d $1
then
	DIR=$1
else
	echo "A directory must be received" ; exit 1
fi

for ELEMENT in $(find $DIR -name "*.sh")
do
	chmod u+x $ELEMENT
done

exit 0;
