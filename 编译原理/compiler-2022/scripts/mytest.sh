#!/bin/bash

WORKDIR=$(dirname "$(dirname "$(readlink -f "$0")")")

build() {
  build_target=$1
  cd "$WORKDIR" && mkdir -p build && cd build && cmake .. >/dev/null && make "$build_target" -j >/dev/null
  if [[ $? != 0 ]]; then
    echo "Error: Compile error, try to run make build and debug"
    exit 1
  fi
}

test() {

  local score_str="LAB5 SCORE"
  local main_script=${WORKDIR}/scripts/lab5_test/main.py
  local testcase_dir=${WORKDIR}/testdata/lab5or6/testcases
  local ref_dir=${WORKDIR}/testdata/lab5or6/refs
  local mergecase_dir=$testcase_dir/merge
  local mergeref_dir=$ref_dir/merge
  local score=0
  local full_score=1
  local testcase_name
  local mergecase_name

  local testcase=$1
  testcase="$testcase_dir"/{$testcase}.tig
  echo testcase
  local testcase_name=$1
  build test_codegen
    # testcase_name=$(basename "$testcase" | cut -f1 -d".")
    local ref=${ref_dir}/${testcase_name}.out
    local assem=$testcase.s
    ./test_codegen "$testcase" >&/dev/null
    if [[ $testcase_name == "merge" ]]; then
      for mergecase in "$mergecase_dir"/*.in; do
        mergecase_name=$(basename "$mergecase" | cut -f1 -d".")
        local mergeref=${mergeref_dir}/${mergecase_name}.out
        python3 ${main_script} ${assem} <"$mergecase" >&${WORKDIR}/output1.txt
        diff -w -B ${WORKDIR}/output1.txt "$mergeref"
        if [[ $? != 0 ]]; then
          echo "Error: Output mismatch [$testcase_name/$mergecase_name]"
          full_score=0
          continue
        fi
        score=$((score + 5))
        echo "Pass $testcase_name/$mergecase_name"
      done
    else
      python3 ${main_script} ${assem} >&${WORKDIR}/output1.txt
      diff -w -B ${WORKDIR}/output1.txt "$ref"
      if [[ $? != 0 ]]; then
        echo "Error: Output mismatch [$testcase_name]"
        full_score=0
        continue
      fi
      echo "Pass $testcase_name"
      score=$((score + 5))
    fi
  
  rm -f "$testcase_dir"/*.tig.s

  if [[ $full_score == 0 ]]; then
    echo "${score_str}: ${score}"
    exit 1
  else
    echo "[^_^]: Pass"
    echo "${score_str}: 100"
  fi
}

test "$1"