#!/bin/bash
# code source: https://linuxhint.com/bash-set-e/
# GCP_IP=35.203.165.222
# GCP_PORT_NUM=80
LOCAL_PORT_NUM=8000
INTEGRATION_TEST_PATH="./integration_testcases"

# setup
function run_setup()
{
	# enter build directory to make server
	cd ../build
	echo "building server..."
	make server &>/dev/null
	cd ../tests

	# generate local server config
	touch local_test_config
	echo "server {" > local_test_config
	echo "listen ${LOCAL_PORT_NUM};" >> local_test_config
	echo "location /static/help StaticHandler { root ./files/images; }" >> local_test_config
	echo "location /static/ StaticHandler { root ./files; }" >> local_test_config
	echo "location /static2/ StaticHandler { root ./files/www; }" >> local_test_config
	echo "}" >> local_test_config
}

function run_cleanup()
{
	rm local_test_config
}

# start local server as background process
function start_local_server()
{
	../build/bin/server local_test_config &>/dev/null &
	local_server_pid=$!
	sleep 1
}

# testing functions

# local server tests
function local_echo_test()
{
	start_local_server
	printf 'GET /echo HTTP/1.1\r\n\r\n' | nc localhost ${LOCAL_PORT_NUM} > /tmp/actual &
	sleep 0.5
	kill ${local_server_pid} #nc process is killed when the server is killed
	diff ${INTEGRATION_TEST_PATH}/expected_echo /tmp/actual
}

function local_bad_request_test()
{
	start_local_server
	printf 'GET /echmo HTTP/1.1\r\n\r\n' | nc localhost ${LOCAL_PORT_NUM} > /tmp/actual &
	sleep 0.5
	kill ${local_server_pid} #nc process is killed when the server is killed
	diff ${INTEGRATION_TEST_PATH}/expected_bad_request /tmp/actual
}

function local_file_not_found_test()
{
	start_local_server
	printf 'GET /static/help/file1.txt HTTP/1.1\r\n\r\n' | nc localhost ${LOCAL_PORT_NUM} > /tmp/actual &
	sleep 0.5
	kill ${local_server_pid} #nc process is killed when the server is killed
	diff ${INTEGRATION_TEST_PATH}/expected_not_found /tmp/actual
}

function local_static_txt_test()
{
	start_local_server
	printf 'GET /static/file1.txt HTTP/1.1\r\n\r\n' | nc localhost ${LOCAL_PORT_NUM} > /tmp/actual &
	sleep 0.5
	kill ${local_server_pid} #nc process is killed when the server is killed
	diff ${INTEGRATION_TEST_PATH}/expected_txt /tmp/actual
}

function local_static_html_test()
{
	start_local_server
	printf 'GET /static/index.html HTTP/1.1\r\n\r\n' | nc localhost ${LOCAL_PORT_NUM} > /tmp/actual &
	sleep 0.5
	kill ${local_server_pid} #nc process is killed when the server is killed
	diff ${INTEGRATION_TEST_PATH}/expected_html /tmp/actual
}

function local_static_zip_test()
{
	start_local_server
	printf 'GET /static2/empty.zip HTTP/1.1\r\n\r\n' | nc localhost ${LOCAL_PORT_NUM} > /tmp/actual &
	sleep 0.5
	kill ${local_server_pid} #nc process is killed when the server is killed
	diff ${INTEGRATION_TEST_PATH}/expected_zip /tmp/actual
}

function local_static_jpg_test()
{
	start_local_server
	printf 'GET /static/help/nyan_cat.jpg HTTP/1.1\r\n\r\n' | nc localhost ${LOCAL_PORT_NUM} > /tmp/actual &
	sleep 0.5
	kill ${local_server_pid} #nc process is killed when the server is killed
	diff ${INTEGRATION_TEST_PATH}/expected_jpg /tmp/actual
}

function local_static_png_test()
{
	start_local_server
	printf 'GET /static/help/hutao.png HTTP/1.1\r\n\r\n' | nc localhost ${LOCAL_PORT_NUM} > /tmp/actual &
	sleep 0.5
	kill ${local_server_pid} #nc process is killed when the server is killed
	diff ${INTEGRATION_TEST_PATH}/expected_png /tmp/actual
}

# to add new tests, add name below
test_list=(local_echo local_bad_request local_file_not_found
	local_static_txt local_static_html local_static_zip
	local_static_jpg local_static_png)

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
