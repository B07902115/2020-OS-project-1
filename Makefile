input_file?=./sample.txt
main=main.c
flag1?=DOGE
flag2?=DONOTSWEAR

all:
	gcc $(main) -o a.out
	./a.out < $(input_file)

debug:
	gcc $(main) -D$(flag1) -D$(flag2) -o a.out
	./a.out < $(input_file) 2> debug.txt

input_debug:
	gcc $(main) -DINPUTDEBUG -o a.out
	./a.out < $(input_file) 2> debug.txt

sched_debug:
	gcc $(main) -DSCHEDDEBUG -o a.out
	./a.out < $(input_file) 2> debug.txt
	
output_debug:
	gcc $(main) -DOUTPUTDEBUG -o a.out
	./a.out < $(input_file) 2> debug.txt
