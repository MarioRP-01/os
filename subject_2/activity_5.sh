#!/bin/bash

if test $# -gt 1 ; then
	echo "Wrong number of parameters"
fi

if test $# -eq 0 ; then
	DIR=$PWD
elif test -d $1 ; then
	DIR=$1
else
	echo "Given file is not a directory"
fi

cd $DIR
du -b `ls` | sort -nr | head -n 2
