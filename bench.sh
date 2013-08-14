#!/bin/bash

# A (work-in-progress) test script for running our benchmarks
# Runs all tests, with timing information

DATECMD="date +%Y-%m-%d-%R"
DATE="`${DATECMD}`"

TESTS="chase-lev-deque/main"
TESTS+=" spsc-queue/spsc-queue"
TESTS+=" spsc-bugfix/spsc-queue"
TESTS+=" barrier/barrier"
TESTS+=" dekker-fences/dekker-fences"
TESTS+=" mcs-lock/mcs-lock"
TESTS+=" mpmc-queue/mpmc-queue-rdwr"
TESTS+=" ms-queue/main"
TESTS+=" linuxrwlocks/linuxrwlocks"

MODEL_ARGS="-y -m 2 -u 3"

#TESTS+=" mpmc-queue/mpmc-2r1w"
#TESTS+=" mpmc-queue/mpmc-1r2w-noinit"
#TESTS+=" mpmc-queue/mpmc-queue-rdwr"
#TESTS+=" mpmc-queue/mpmc-queue-noinit"

COUNT=0

function run_test {
	t=$1
	shift
	ARGS="$@"
	RUN="./run.sh"

	echo "-----------------------------------------------"
	echo "*******************************"
	echo "Running test ${COUNT} (${t})"
	echo "ARGS=${ARGS}"
	echo "*******************************"
	(time ${RUN} ${t} ${ARGS} 2>&1) 2>&1
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

# Check if git is available, and this is a git repository
GIT=0
which git &> /dev/null && git rev-parse &> /dev/null && GIT=1

# Print out some git information, if available
if [ ${GIT} -ne 0 ]; then
	cd ..
	git log --oneline -1
	cd - > /dev/null
	git log --oneline -1
	echo
fi
run_all_tests
