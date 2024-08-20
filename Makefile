ROTATE_NAME := rotate_2d_array
HTTP_SERVE := http_server

build/test: 
	clang ./test.c -o /tmp/test -lm

run/test: build/test
	/tmp/test

build/rotate: 
	clang ./${ROTATE_NAME}.c -o /tmp/${ROTATE_NAME} -lm

run/rotate: build/rotate
	/tmp/${ROTATE_NAME}

build/server: 
	gcc ./http/${HTTP_SERVE}.c ./http/http.c -o /tmp/${HTTP_SERVE} -lm

run/server: build/server
	/tmp/${HTTP_SERVE}