#!/bin/bash

WORKDIR=$(dirname "$(dirname "$(readlink -f "$0")")")

main(){
  rm ${WORKDIR}/src/tiger/lex/lex.cc
  rm ${WORKDIR}/src/tiger/lex/scannerbase.h
  rm ${WORKDIR}/src/tiger/parse/parse.cc
  rm ${WORKDIR}/src/tiger/parse/parserbase.h

}

main