input_file?=./sample.txt
main=main.c

all:
	gcc $(main) -o a.out
	./a.out < $(input_file)

debug:
	gcc $(main) -DDEBUG -o a.out
	./a.out < $(input_file) 2> debug.txt

idebug:
	gcc $(main) -DINPUTDEBUG -o a.out
	./a.out < $(input_file) 2> debug.txt
	
