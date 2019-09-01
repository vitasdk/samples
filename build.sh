#!/bin/sh

SCRIPTNAME='build.sh'
RC=0
if [ -n "${BASH:-}" ]; then
  [ "${0}" = "${BASH_SOURCE}" ] || RC=1
else
  [ "$(basename -- "${0}")" = "${SCRIPTNAME}" ] || RC=1
fi
[ "${RC}" -eq 0 ] || {
  printf -- 'Script cannot be sourced\n'
  return 1 || exit 1
}

[ -d 'build' ] || mkdir build
cd build
cmake .. && make
