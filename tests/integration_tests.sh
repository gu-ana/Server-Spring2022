#!/bin/bash
# code source: https://linuxhint.com/bash-set-e/
GCP_IP=35.203.165.222
GCP_PORT_NUM=80
LOCAL_PORT_NUM=8080

# setup
function run_setup() {
	# enter build directory to make server
	cd ../build
	echo "building server..."
	make server &>/dev/null
	cd ../tests

	# generate local server config
	touch local_test_config
	echo "listen ${LOCAL_PORT_NUM};" >> local_test_config
}

function run_cleanup() {
	rm local_test_config
}

# start local server as background process
function start_local_server() {
	../build/bin/server local_test_config &>/dev/null &
	local_server_pid=$!
	sleep 1
}

# testing functions
function local_response_header_test() {
	start_local_server
	curl -s -S -I http://localhost:${LOCAL_PORT_NUM} -o /tmp/actual
	kill ${local_server_pid}
	diff expected_local_header /tmp/actual
}

function local_full_response_test() {
	start_local_server
	curl -s -S -i http://localhost:${LOCAL_PORT_NUM} -o /tmp/actual
	kill ${local_server_pid}
	diff expected_local_full /tmp/actual
}

function gcp_response_header_test() {
	curl -s -S -I ${GCP_IP}:${PORT_NUM} -o /tmp/actual
	diff expected_gcp_header /tmp/actual
}

function gcp_full_response_test() {
	curl -s -S -i ${GCP_IP}:${PORT_NUM} -o /tmp/actual
	diff expected_gcp_full /tmp/actual
}

# to add new tests, add name below
test_list=(local_response_header local_full_response
	gcp_response_header gcp_full_response)

# run setup commands
run_setup

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

# clean up test artifacts
run_cleanup

# report how many tests passed
echo "${passed} of ${num_tests} tests passed"

if [ ${passed} -eq ${num_tests} ]
then
	exit 0
else
	exit 1
fi
