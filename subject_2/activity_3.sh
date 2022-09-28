#!/bin/bash

if test $# -gt 1
then
	echo Wrong number of parameters ; exit 1;
fi

if test $# -eq 0
then
	DIR=$PWD
elif test -d $1
then
	DIR=$1
else
	echo A directory is required ; exit 1;
fi

find $DIR -name "[ab]*" | grep -v "~"

exit 0;

