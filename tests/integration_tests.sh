#!/bin/bash
# code source: https://linuxhint.com/bash-set-e/
# GCP_IP=35.203.165.222
# GCP_PORT_NUM=80
LOCAL_PORT_NUM=8000
INTEGRATION_TEST_PATH="./integration_testcases"
API_TEST_PATH="./mnt"

# setup
function run_setup()
{
	# enter build directory to make server
	cd ../build
	echo "building server..."
	make -j 4 server &>/dev/null
	cd ../tests

	# generate local server config
	touch local_test_config
	echo "server {" > local_test_config
	echo "listen ${LOCAL_PORT_NUM};" >> local_test_config
	echo "location /static/help StaticHandler { root ./files/images; }" >> local_test_config
	echo "location /static/ StaticHandler { root ./files; }" >> local_test_config
	echo "location /static2/ StaticHandler { root ./files/www; }" >> local_test_config
	echo "location /api/ ApiHandler { data_path ./mnt; }" >> local_test_config
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

function local_api_test()
{
	mkdir ${API_TEST_PATH}
	start_local_server

	# POST
	# valid POST
	json_payload='{"data":1}'
	post_res="$(curl -s -X POST http://localhost:${LOCAL_PORT_NUM}/api/shoes -d $json_payload)"
	expected_post_res='{"id": 1}'
	if [[ "$expected_post_res" != "$post_res" ]];
	then
		echo "Failed valid POST"
		echo "Expected Response: $expected_post_res"
		echo "Actual Response: $post_res"
		kill ${local_server_pid}
		rm -r ${API_TEST_PATH}
		return 1
	fi

	file_contents="$(cat ${API_TEST_PATH}/shoes/1)"
	if [[ "$json_payload" != "$file_contents" ]];
    then
        echo "Failed valid POST"
		echo "Expected File Contents: $json_payload"
        echo "Actual File Contents: $file_contents"
        kill ${local_server_pid}
		# rm -r ${API_TEST_PATH}
        return 1
    fi

	# GET
	# valid GET id
    get_res="$(curl -s -X GET http://localhost:${LOCAL_PORT_NUM}/api/shoes/1)"
    expected_get_res="$(cat ${API_TEST_PATH}/shoes/1)"
    if [[ "$get_res" != "$file_contents" ]];
    then
        echo "Failed valid GET id"
		echo "Expected Response: $expected_get_res"
		echo "Actual Response: $get_res"
        kill ${local_server_pid}
		rm -r ${API_TEST_PATH}
        return 1
    fi

	# valid GET list
	get_res="$(curl -s -X GET http://localhost:${LOCAL_PORT_NUM}/api/shoes)"
	expected_get_res='[1]'
	if [[ "$expected_get_res" != "$get_res" ]];
	then
		echo "Failed valid GET list"
		echo "Expected Response: $expected_get_res"
		echo "Actual Response: $get_res"
		kill ${local_server_pid}
		rm -r ${API_TEST_PATH}
		return 1
	fi

	# PUT
	# valid PUT update
	json_payload='{"data":2}'
	put_res="$(curl -s -X PUT http://localhost:${LOCAL_PORT_NUM}/api/shoes/1 -d $json_payload)"
	expected_put_res='{"id": 1}'
    if [[ "$put_res" != "$expected_put_res" ]];
    then
        echo "Failed valid PUT update"
		echo "Expected Response: $expected_put_res"
		echo "Actual Response: $put_res"
        kill ${local_server_pid}
		rm -r ${API_TEST_PATH}
        return 1
    fi

	file_contents="$(cat ${API_TEST_PATH}/shoes/1)"
	if [[ "$json_payload" != "$file_contents" ]];
    then
		echo "Failed valid PUT update"
		echo "Expected File Contents: $json_payload"
        echo "Actual File Contents: $file_contents"
        kill ${local_server_pid}
		rm -r ${API_TEST_PATH}
        return 1
    fi

	# valid PUT create
	json_payload='{"data":3}'
	put_res="$(curl -s -X PUT http://localhost:${LOCAL_PORT_NUM}/api/shoes/2 -d $json_payload)"
	expected_put_res='{"id": 2}'
    if [[ "$put_res" != "$expected_put_res" ]];
    then
		echo "Failed valid PUT create"
		echo "Expected Response: $expected_put_res"
		echo "Actual Response: $put_res"
        kill ${local_server_pid}
		rm -r ${API_TEST_PATH}
        return 1
    fi

	file_contents="$(cat ${API_TEST_PATH}/shoes/2)"
	if [[ "$json_payload" != "$file_contents" ]];
    then
		echo "Failed valid PUT create"
		echo "Expected File Contents: $json_payload"
        echo "Actual File Contents: $file_contents"
        kill ${local_server_pid}
		rm -r ${API_TEST_PATH}
        return 1
    fi

	# DELETE
	# valid DELETE
	curl -X DELETE http://localhost:${LOCAL_PORT_NUM}/api/shoes/1 &>/dev/null
	
	if [[ -f "${API_TEST_PATH}/shoes/1" ]]; # checks if file still exists
	then
		echo "Failed valid DELETE"
		echo "File still exists"
		kill ${local_server_pid}
		rm -r ${API_TEST_PATH}
		return 1
	fi
	
	kill ${local_server_pid}
	rm -r ${API_TEST_PATH}
	
}

function local_multithreading_test()
{
	start_local_server
	touch responses

	curl -s -X GET http://localhost:${LOCAL_PORT_NUM}/sleep >> responses &
	curl -s -X GET http://localhost:${LOCAL_PORT_NUM}/echo >> responses &
	sleep 2

	kill ${local_server_pid}

	# get line number of echo response
	echo_response=`cat responses | grep -n 'GET /echo' | grep -Po '^([0-9])+'`
	# get line number of sleep response
	sleep_response=`cat responses | grep -n 'Thread slept' | grep -Po '^([0-9])+'`

	rm responses

	# check if response for /echo was received before /sleep
	if [[ $echo_response -lt $sleep_response ]]
	then
		return 0
	else
		return 1
	fi
}

function local_health_test()
{
	start_local_server
	printf 'GET /health HTTP/1.1\r\n\r\n' | nc localhost ${LOCAL_PORT_NUM} > /tmp/actual &
	sleep 0.5
	kill ${local_server_pid} #nc process is killed when the server is killed
	diff ${INTEGRATION_TEST_PATH}/expected_health /tmp/actual
}

# to add new tests, add name below
test_list=(local_echo local_bad_request local_file_not_found
	local_static_txt local_static_html local_static_zip
	local_static_jpg local_static_png local_api local_multithreading
	local_health)

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
