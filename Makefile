input_file?=/dev/tty

all:
	gcc project1_test.c -o a.out
	./a.out < $(input_file)

idebug:
	gcc project1_test.c -DINPUTDEBUG -o a.out
	./a.out < $(input_file) 2> debug.txt
	