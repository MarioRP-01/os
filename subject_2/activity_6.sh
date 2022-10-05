#!/bin/bash

if test $# -ne 1 ; then
  echo "Wrong number of parameters"; exit 1
fi

if ! test -f $1 ; then
  echo "Given parameter must be a file"; exit 2
fi

clear_temp() {
  if test -d $1 ; then
    rm -rf $1
  fi
}

extract() {
  if test -d $2 ; then
    tar -xf $1 -C $2
  fi
}

remove_bigger() {
  if ! test -d $1 ; then
    return
  fi
  
  for file in $(ls $1) ; do
    file_size=$(wc -c $file | cut -d " " -f 1)        

    if test $file_size -gt 8000 ; then
      rm $temp"/"$file
      echo Archivo borrado: $file
    else
      echo Archivo guardado: $file
    fi
  done
} 

compress() {
  if ! test -d $1 ; then
    return
  fi
  echo $1"/*"
  tar -cf comp.tar $1"/*"
}

file_comp=$1
temp=$(mktemp -dt activity_6_XXX)

# I don't understand why this doesn't get my parameters
extract $file_comp $temp
ls $temp

remove_bigger $temp
ls $temp

compress $temp
ls

clear_temp $temp
