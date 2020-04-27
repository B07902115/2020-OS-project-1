input_file?=./sample.txt
main=main.c
flag1?=DOGE
flag2?=DONOTSWEAR
test_files=OS_PJ1_Test
demo_files=TIME_MEASUREMENT FIFO_1 PSJF_2 RR_3 SJF_4
output_dir=output

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

test:
	@echo
	dmesg --clear
	./a.out < $(test_files)/$(input_file).txt > $(output_dir)/$(input_file)_output.txt
	dmesg | grep project1 > $(output_dir)/$(input_file)_dmesg.txt
	@echo

generate_test:
	gcc $(main) -o a.out
	for i in `ls $(test_files)`; do make test input_file=$$(basename $$i .txt); done;

demo_test:
	@echo
	dmesg --clear
	./a.out < $(test_files)/$(input_file).txt
	dmesg | grep project1
	@echo

generate_demo:
	for i in $(demo_files); do make demo_test input_file=$$i; done;
