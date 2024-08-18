ROTATE_NAME := rotate_2d_array

build/rotate: 
	clang ./${ROTATE_NAME}.c -o /tmp/${ROTATE_NAME} -lm

run/rotate: build/rotate
	/tmp/${ROTATE_NAME}