#!/bin/bash

if test $# -gt 1 ; then
  echo "Wrong number of parameters; One or none directories must be specified" ; exit 1
fi

if test $# -eq 0 ; then
  dir=$PWD
elif test -d $1 ; then
  dir=$1
else
  echo "Given parameter must be a directory" ; exit 2
fi

count=0

for file in $(find $dir -type f -perm /u+x -name "*.sh"); do
  grep bash $file
  count=$(($count+1))
done

echo Se han encontrado $count ficheros con extension .sh y permiso de ejecuion para el usuario.
exit 0

