#!/bin/bash

if test $# -gt 2 ; then
  echo Cagaste, pa la próxima menos argumentos ; exit 1
fi

if test $# -eq 0 ; then
  echo Cagaste, pa la proxima pon entre 1 y 2 argumentos ; exit 1
fi

if test $# -eq 2 ; then
  if test -d $2 ; then
    dest_dir=$2
  else 
    echo Cagaste, por un directorio para la proxima bobalicon; exit 2
  fi
else
  dest_dir=$PWD
fi

if test -d $1 ; then
  ori_dir=$1
else
  echo Cagaste, pon un directorio para la proxima ; exit 2 
fi

for file in $(find $ori_dir -type f -name "*.mp3"); do
  cp $file $dest_dir
done

exit 0
