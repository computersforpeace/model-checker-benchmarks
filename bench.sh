#!/bin/bash

# A (work-in-progress) test script for running our benchmarks
# Runs all tests, logging output to a directory named
# '${BASEDIR}/run-<date-time>', where ${BASEDIR} is either the current
# directory or the first parameter to this script

## Unfinished benchmarks - do not use
# queue williams-queue

DATECMD="date +%Y-%m-%d-%R"
DATE="`${DATECMD}`"
BASEDIR=.

[ $# -gt 0 ] && [ -d "$1" ] && BASEDIR="$1" && shift

DIR="${BASEDIR}/run-${DATE}"

TESTS="barrier/barrier"
TESTS+=" mcs-lock/mcs-lock"
TESTS+=" spsc-queue/spsc-queue"
TESTS+=" spsc-bugfix/spsc-queue"
TESTS+=" dekker-fences/dekker-fences"
TESTS+=" mpmc-queue/mpmc-2r1w"
TESTS+=" mpmc-queue/mpmc-1r2w-noinit"
TESTS+=" mpmc-queue/mpmc-queue-noinit"
TESTS+=" linuxrwlocks/linuxrwlocks"

MODEL_ARGS="-y -m 2"
COUNT=0

function run_test {
	t=$1
	shift
	ARGS="$@"
	RUN="./run.sh"
	LOG=${DIR}/log-${COUNT}

	echo "-----------------------------------------------"
	echo "*******************************"
	echo "Running test ${COUNT} (${t}): logging to ${LOG}"
	echo "ARGS=${ARGS}"
	echo "*******************************"
	(time ${RUN} ${t} ${ARGS} 2>&1) 2>&1 | tee ${LOG}
	echo
	echo "Test done; sleeping for a few seconds"
	echo

	let COUNT++
}

function run_all_tests {
	echo ${DATE}

	for t in ${TESTS}
	do
		run_test ${t} ${MODEL_ARGS}
	done
	#run_test mpmc-queue/mpmc-queue ${MODEL_ARGS} -- -r 2 -w 1
	#run_test mpmc-queue/mpmc-queue ${MODEL_ARGS} -- -r 1 -w 2
	#run_test mpmc-queue/mpmc-queue ${MODEL_ARGS} -- -r 2 -w 2
}

mkdir ${DIR}
(cd ..; git log --oneline -1; cd - > /dev/null; git log --oneline -1; echo; run_all_tests) | tee ${DIR}/timing.log
