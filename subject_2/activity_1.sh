#!/bin/bash

# This program check which commands alocated in /bin exist in the page 1 of man.



if test $# -ne 0
then
	echo "This program do not use parameters." ; exit 1;
fi

for FILE_NAME in $(ls /bin)
do
	MAN_ROUTE="/usr/share/man/man1/"$FILE_NAME".1.gz";
	
	if test -e $MAN_ROUTE
	then
		echo $FILE_NAME;
	fi
done
