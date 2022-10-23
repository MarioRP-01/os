#!/bin/bash

if test $# -ne 0 ; then
  echo Cagaste, para la proxima no pongas ni un argumento ; exit 1
fi

echo Espacio total ocupado: `du -md 0 $HOME | cut -f 1`M

echo 

echo Lista de directorios segun tamaño \(K\):
du -kS $HOME | sort -nr | head -5

echo Lista de ficheron por tamaño:
du -abt 3000000 `find $HOME -type f` 2> /dev/null | sort -nr | head -10

exit 0
