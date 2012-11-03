#!/bin/bash

# A (work-in-progress) test script for running our benchmarks
# Runs all tests, logging output to a directory named 'run-<date-time>'

## Unfinished benchmarks - do not use
# queue williams-queue

DATECMD="date +%Y-%m-%d-%R"
DATE="`${DATECMD}`"
DIR="run-${DATE}"

TESTS="barrier/barrier mcs-lock/mcs-lock spsc-queue/spsc-queue mpmc-queue/mpmc-1r2w mpmc-queue/mpmc-2r1w mpmc-queue/mpmc-queue linuxrwlocks/linuxrwlocks"
MODEL_ARGS="-f 4 -m 2"
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
	time ${RUN} ${t} ${ARGS} > ${LOG} 2>&1
	echo
	grep -A 2 "Number of executions" ${LOG} | tail -3
	echo
	echo "Test done; sleeping for a few seconds"
	echo
	sleep 3

	echo "*******************************"
	echo "Re-running test for timing data"
	echo "*******************************"
	time ${RUN} ${t} ${ARGS} > /dev/null 2>&1
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
(git log --oneline -1; echo; run_all_tests) | tee ${DIR}/timing.log
