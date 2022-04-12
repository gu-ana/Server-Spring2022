#!/bin/bash
# code source: https://linuxhint.com/bash-set-e/
GCP_IP=35.203.165.222
GCP_PORT_NUM=80
LOCAL_PORT_NUM=8080

# testing functions
function local_response_header_test() {
	../build/bin/server local_test_config &>/dev/null &
	curl -s -I http://localhost:${LOCAL_PORT_NUM} > /tmp/actual
	diff expected_local_header /tmp/actual
}

function local_full_response_test() {
	../build/bin/server local_test_config &>/dev/null &
	curl -s -i http://localhost:${LOCAL_PORT_NUM} > /tmp/actual
	diff expected_local_full /tmp/actual
}

function gcp_response_header_test() {
	curl -s -I ${GCP_IP}:${PORT_NUM} > /tmp/actual
	diff expected_gcp_header /tmp/actual
}

function gcp_full_response_test() {
	curl -s -i ${GCP_IP}:${PORT_NUM} > /tmp/actual
	diff expected_gcp_full /tmp/actual
}

# to add new tests, add name below
test_list=(local_response_header local_full_response
	gcp_response_header gcp_full_response)

# enter build directory to make server
cd ../build
echo "building server..."
make server &>/dev/null
cd ../tests

# run tests
num_tests=${#test_list[@]}
passed=0

for item in ${test_list[@]}
do
	echo "running ${item}_test..."
	${item}_test # call test function

	if [ $? -eq 0 ] # check exit code
	then
		echo "PASSED"
		let passed=passed+1
	else
		echo "FAILED"
	fi
done

# report how many tests passed
echo "${passed} of ${num_tests} tests passed"
