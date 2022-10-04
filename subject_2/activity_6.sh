#!/bin/bash

if test $# -ne 1 ; then
  echo "Wrong number of parameters"; exit 1
fi

if ! test -f $1 ; then
  echo "Given parameter must be a file"; exi
fi

clear_temp() {
  if test -d $1 ; then
    rm -rf $1
  fi
}

temp=$(mktemp -dt activity_6_XXX)

clear_temp $temp
