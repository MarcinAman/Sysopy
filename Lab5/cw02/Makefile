compile: main.c
	make clean
	gcc -o main main.c
	gcc -o slave slave.c
	gcc -o master master.c
clean:
	rm -f slave main master sample_fifo
test:
	./main sample_fifo 10 5
all: compile test
